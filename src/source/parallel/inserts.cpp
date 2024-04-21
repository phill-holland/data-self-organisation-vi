#include "parallel/inserts.hpp"

sycl::int4 MapClientIdx(const int index, const sycl::int4 dimensions)
{
    int n = dimensions.x() * dimensions.y();
    int r = index % n;
    float z = (float)((index / n));

    int j = r % dimensions.x();
    float y = (float)((r / dimensions.x()));
    float x = (float)(j);

    return { x, y, z, index };
}

void organisation::parallel::inserts::reset(::parallel::device &dev, 
                                            ::parallel::queue *q, 
                                            parameters &settings)
{
    init = false; cleanup();

    this->dev = &dev;
    this->queue = q;
    this->settings = settings;
    this->length = settings.max_inserts * settings.clients();

    sycl::queue &qt = ::parallel::queue(dev).get();

    deviceNewPositions = sycl::malloc_device<sycl::float4>(length, qt);
    if(deviceNewPositions == NULL) return;

    deviceNewValues = sycl::malloc_device<sycl::int4>(length, qt);
    if(deviceNewValues == NULL) return;

    deviceNewMovementPatternIdx = sycl::malloc_device<int>(length, qt);
    if(deviceNewMovementPatternIdx == NULL) return;

    deviceNewClient = sycl::malloc_device<sycl::int4>(length, qt);
    if(deviceNewClient == NULL) return;

    deviceInputData = sycl::malloc_device<int>(settings.max_input_data * settings.epochs(), qt);
    if(deviceInputData == NULL) return;

    deviceInsertsDelay = sycl::malloc_device<int>(settings.max_inserts * settings.clients(), qt);
    if(deviceInsertsDelay == NULL) return;

    deviceInsertsLoops = sycl::malloc_device<int>(settings.max_inserts * settings.clients(), qt);
    if(deviceInsertsLoops == NULL) return;

    deviceInsertsStartingPosition = sycl::malloc_device<sycl::float4>(settings.max_inserts * settings.clients(), qt);
    if(deviceInsertsStartingPosition == NULL) return;

    deviceInsertsMovementPatternIdx = sycl::malloc_device<int>(settings.max_inserts * settings.clients(), qt);
    if(deviceInsertsMovementPatternIdx == NULL) return;

    deviceInsertsWords = sycl::malloc_device<int>(settings.max_inserts * settings.clients(), qt);
    if(deviceInsertsWords == NULL) return;

    deviceMovements = sycl::malloc_device<sycl::float4>(settings.max_movements * settings.max_movement_patterns * settings.clients(), qt);
    if(deviceMovements == NULL) return;

    deviceMovementsCounts = sycl::malloc_device<int>(settings.max_movement_patterns * settings.clients(), qt);
    if(deviceMovementsCounts == NULL) return;

    deviceInputIdx = sycl::malloc_device<int>(settings.clients(), qt);
    if(deviceInputIdx == NULL) return;

    deviceTotalNewInserts = sycl::malloc_device<int>(1, qt);
    if(deviceTotalNewInserts == NULL) return;

    hostTotalNewInserts = sycl::malloc_host<int>(1, qt);
    if(hostTotalNewInserts == NULL) return;

    hostInputData = sycl::malloc_host<int>(settings.max_input_data * settings.input.size(), qt);
    if(hostInputData == NULL) return;

    hostInsertsDelay = sycl::malloc_host<int>(settings.max_inserts * settings.host_buffer, qt);
    if(hostInsertsDelay == NULL) return;

    hostInsertsLoops = sycl::malloc_host<int>(settings.max_inserts * settings.host_buffer, qt);
    if(hostInsertsLoops == NULL) return;

    hostInsertsStartingPosition = sycl::malloc_host<sycl::float4>(settings.max_inserts * settings.host_buffer, qt);
    if(hostInsertsStartingPosition == NULL) return;

    hostInsertsMovementPatternIdx = sycl::malloc_host<int>(settings.max_inserts * settings.host_buffer, qt);
    if(hostInsertsMovementPatternIdx == NULL) return;

    hostInsertsWords = sycl::malloc_host<int>(settings.max_inserts * settings.host_buffer, qt);
    if(hostInsertsWords == NULL) return;

    hostMovements = sycl::malloc_host<sycl::float4>(settings.max_movements * settings.max_movement_patterns * settings.host_buffer, qt);
    if(hostMovements == NULL) return;

    hostMovementsCounts = sycl::malloc_host<int>(settings.max_movement_patterns * settings.host_buffer, qt);
    if(hostMovementsCounts == NULL) return;

    clear();

    init = true;
}

void organisation::parallel::inserts::restart()
{
    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);

    std::vector<sycl::event> events;

    events.push_back(qt.memset(deviceNewPositions, 0, sizeof(sycl::float4) * length));
    events.push_back(qt.memset(deviceNewValues, -1, sizeof(sycl::int4) * length));
    events.push_back(qt.memset(deviceNewClient, 0, sizeof(sycl::int4) * length));
    events.push_back(qt.memset(deviceInputIdx, 0, sizeof(int) * settings.clients()));
    
    sycl::event::wait(events);
}

void organisation::parallel::inserts::clear()
{
    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);

    std::vector<sycl::event> events;

    events.push_back(qt.memset(deviceNewPositions, 0, sizeof(sycl::float4) * length));
    events.push_back(qt.memset(deviceNewValues, -1, sizeof(sycl::int4) * length));
    events.push_back(qt.memset(deviceNewClient, 0, sizeof(sycl::int4) * length));
    events.push_back(qt.memset(deviceInputIdx, 0, sizeof(int) * settings.clients()));
    events.push_back(qt.memset(deviceMovements, 0, sizeof(sycl::float4) * settings.max_movements * settings.max_movement_patterns * settings.clients()));
    events.push_back(qt.memset(deviceMovementsCounts, 0, sizeof(int) * settings.max_movement_patterns * settings.clients()));
    events.push_back(qt.memset(deviceInsertsDelay, 0, sizeof(int) * settings.max_inserts * settings.clients()));    
    events.push_back(qt.memset(deviceInsertsLoops, 0, sizeof(int) * settings.max_inserts * settings.clients()));

    sycl::event::wait(events);
}

int organisation::parallel::inserts::insert(int epoch, int iteration)
{
    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);
    sycl::range num_items{(size_t)settings.clients()};

    qt.memset(deviceTotalNewInserts, 0, sizeof(int)).wait();

    auto epoch_offset = (epoch > settings.epochs() ? settings.epochs() : epoch) * settings.max_input_data;

    sycl::float4 starting = { (float)settings.starting.x, (float)settings.starting.y, (float)settings.starting.z, 0.0f };
    sycl::int4 dim_clients = { settings.dim_clients.x, settings.dim_clients.y, settings.dim_clients.z, 0.0f };

    qt.submit([&](auto &h) 
    {        
        auto _insertsDelay = deviceInsertsDelay;

        auto _insertsStartingPosition = deviceInsertsStartingPosition;
        auto _insertsMovementPatternIdx = deviceInsertsMovementPatternIdx;
        auto _insertsWords = deviceInsertsWords;

        auto _inputData = deviceInputData;
        auto _inputIdx = deviceInputIdx;

        auto _totalNewInserts = deviceTotalNewInserts;
        
        auto _values = deviceNewValues;
        auto _positions = deviceNewPositions;
        auto _movementPatternIdx = deviceNewMovementPatternIdx;
        auto _clients = deviceNewClient;

        auto _epoch_offset = epoch_offset;

        auto _max_inserts = settings.max_inserts;
        auto _dim_clients = dim_clients;

        auto _iteration = iteration + 1;
        auto _length = length;

        h.parallel_for(num_items, [=](auto client) 
        {
            int offset = (client * _max_inserts);

            for(int i = 0; i < _max_inserts; ++i)
            {
                if(_insertsMovementPatternIdx[i + offset] != -1)
                {
                    if(_inputData[_inputIdx[client] + epoch_offset] == -1) return;
                                        
                    int delay = _insertsDelay[i + offset] + 1;

                    if(_iteration % delay == 0)
                    {     
                        sycl::int4 new_value = { -1, -1, -1, -1 };
                        int *coordinates[] = { &new_value.x(), &new_value.y(), &new_value.z() } ;

                        int words = _insertsWords[i + offset];                        
                        if(words > 3) words = 3;
                        if(words < 1) words = 1;
                        int word_index = 0;
                        while((word_index < words)&&(_inputData[_inputIdx[client] + epoch_offset] != -1))
                        {
                            int b = _inputIdx[client];
                    
                            *coordinates[word_index++] = _inputData[b + epoch_offset];
                            _inputIdx[client]++;
                        };
                        
                        cl::sycl::atomic_ref<int, cl::sycl::memory_order::relaxed, 
                                                    sycl::memory_scope::device, 
                                                    sycl::access::address_space::ext_intel_global_device_space> ar(_totalNewInserts[0]);

                        int dest = ar.fetch_add(1);
                        if(dest < _length)                
                        {                    
                            _values[dest] = new_value;
                            _positions[dest] = _insertsStartingPosition[offset + i];
                            _movementPatternIdx[dest] = _insertsMovementPatternIdx[offset + i];

                            _clients[dest] = MapClientIdx(client, _dim_clients);
                        }
                    }
                }
            }
        });
    }).wait();

    qt.memcpy(hostTotalNewInserts, deviceTotalNewInserts, sizeof(int)).wait();

    return hostTotalNewInserts[0];
}

void organisation::parallel::inserts::set(organisation::data &mappings, inputs::input &source)
{
    memset(hostInputData, -1, sizeof(int) * settings.max_input_data * settings.epochs());

    int offset = 0;
    for(int i = 0; i < source.size(); ++i)
    {        
        inputs::epoch epoch;
        if(source.get(epoch, i))
        {
            std::vector<int> temp = mappings.get(epoch.input);
            int len = temp.size();
            if(len > settings.max_input_data) len = settings.max_input_data;

            for(int j = 0; j < len; ++j)
            {
                hostInputData[j + offset] = temp[j];
            }

            offset += settings.max_input_data;
        }
    }
    
    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);
    qt.memcpy(deviceInputData, hostInputData, sizeof(int) * settings.max_input_data * settings.epochs()).wait();
}

std::vector<organisation::parallel::value> organisation::parallel::inserts::get()
{
    std::vector<value> result;

    int totals = hostTotalNewInserts[0];
    if(totals > 0)
    {
        std::vector<sycl::int4> values = dev->get(deviceNewValues, totals);
        std::vector<sycl::int4> clients = dev->get(deviceNewClient, totals);
        std::vector<sycl::float4> positions = dev->get(deviceNewPositions, totals);

        if((values.size() == totals)&&(clients.size() == totals)&&(positions.size() == totals))
        {
            int len = values.size();
            for(int i = 0; i < len; ++i)
            {
                value temp;

                temp.data = point(values[i].x(),values[i].y(),values[i].z());
                temp.client = clients[i].w();
                temp.position = point(positions[i].x(), positions[i].y(), positions[i].z());

                result.push_back(temp);
            }
        }
    }

    return result;
}

void organisation::parallel::inserts::copy(::organisation::schema **source, int source_size)
{
    memset(hostInsertsDelay, -1, sizeof(int) * settings.max_inserts * settings.host_buffer);
    memset(hostInsertsLoops, -1, sizeof(int) * settings.max_inserts * settings.host_buffer);
    memset(hostInsertsStartingPosition, 0, sizeof(sycl::float4) * settings.max_inserts * settings.host_buffer);
    memset(hostInsertsMovementPatternIdx, -1, sizeof(int) * settings.max_inserts * settings.host_buffer);
    memset(hostInsertsWords, 0, sizeof(int) * settings.max_inserts * settings.host_buffer);

    memset(hostMovements, 0, sizeof(sycl::float4) * settings.max_movements * settings.max_movement_patterns * settings.host_buffer);
    memset(hostMovementsCounts, 0, sizeof(int) * settings.max_movement_patterns * settings.host_buffer);

    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);
    sycl::range num_items{(size_t)settings.clients()};

    int dest_index = 0;
    int index = 0;

    for(int source_index = 0; source_index < source_size; ++source_index)
    {
        organisation::program *prog = &source[source_index]->prog;

        int pattern = 0;
        for(auto &it: prog->insert.values)
        {
            hostInsertsDelay[pattern + (index * settings.max_inserts)] = it.delay;
            hostInsertsLoops[pattern + (index * settings.max_inserts)] = it.loops;
            hostInsertsStartingPosition[pattern + (index * settings.max_inserts)] = { (float)it.starting.x, (float)it.starting.y, (float)it.starting.z, 0.0f };
            hostInsertsMovementPatternIdx[pattern + (index * settings.max_inserts)] = pattern;
            hostInsertsWords[pattern + (index * settings.max_inserts)] = it.words;
            
            for(auto &direction: it.movement.directions)
            {            
                int m_count = hostMovementsCounts[(index * settings.max_movement_patterns) + pattern];
                
                if(m_count < settings.max_movements)
                {
                    int offset = (pattern * settings.max_movements) + m_count;
                    hostMovements[(index * settings.max_movements * settings.max_movement_patterns) + offset] = { (float)direction.x, (float)direction.y, (float)direction.z, 0.0f };
                    hostMovementsCounts[(index * settings.max_movement_patterns) + pattern] += 1;
                }            
            }

            ++pattern;
            if(pattern >= settings.max_movement_patterns) break;
        }
        
        ++index;
        if(index >= settings.host_buffer)
        {
            std::vector<sycl::event> events;

            events.push_back(qt.memcpy(&deviceInsertsDelay[dest_index * settings.max_inserts], hostInsertsDelay, sizeof(int) * settings.max_inserts * index));
            events.push_back(qt.memcpy(&deviceInsertsLoops[dest_index * settings.max_inserts], hostInsertsLoops, sizeof(int) * settings.max_inserts * index));
            events.push_back(qt.memcpy(&deviceInsertsStartingPosition[dest_index * settings.max_inserts], hostInsertsStartingPosition, sizeof(sycl::float4) * settings.max_inserts * index));
            events.push_back(qt.memcpy(&deviceInsertsMovementPatternIdx[dest_index * settings.max_inserts], hostInsertsMovementPatternIdx, sizeof(int) * settings.max_inserts * index));
            events.push_back(qt.memcpy(&deviceInsertsWords[dest_index * settings.max_inserts], hostInsertsWords, sizeof(int) * settings.max_inserts * index));
            events.push_back(qt.memcpy(&deviceMovements[dest_index * settings.max_movements * settings.max_movement_patterns], hostMovements, sizeof(sycl::float4) * settings.max_movements * settings.max_movement_patterns * index));
            events.push_back(qt.memcpy(&deviceMovementsCounts[dest_index * settings.max_movement_patterns], hostMovementsCounts, sizeof(int) * settings.max_movement_patterns * index));

            sycl::event::wait(events);

            memset(hostInsertsDelay, -1, sizeof(int) * settings.max_inserts * settings.host_buffer);
            memset(hostInsertsLoops, -1, sizeof(int) * settings.max_inserts * settings.host_buffer);
            memset(hostInsertsStartingPosition, 0, sizeof(sycl::float4) * settings.max_inserts * settings.host_buffer);
            memset(hostInsertsMovementPatternIdx, -1, sizeof(int) * settings.max_inserts * settings.host_buffer);
            memset(hostInsertsWords, 0, sizeof(int) * settings.max_inserts * settings.host_buffer);
            memset(hostMovements, 0, sizeof(sycl::float4) * settings.max_movements * settings.max_movement_patterns * settings.host_buffer);
            memset(hostMovementsCounts, 0, sizeof(int) * settings.max_movement_patterns * settings.host_buffer);

            dest_index += settings.host_buffer;
            index = 0;            
        }
    }

    if(index > 0)
    {
        std::vector<sycl::event> events;

        events.push_back(qt.memcpy(&deviceInsertsDelay[dest_index * settings.max_inserts], hostInsertsDelay, sizeof(int) * settings.max_inserts * index));
        events.push_back(qt.memcpy(&deviceInsertsLoops[dest_index * settings.max_inserts], hostInsertsLoops, sizeof(int) * settings.max_inserts * index));
        events.push_back(qt.memcpy(&deviceInsertsStartingPosition[dest_index * settings.max_inserts], hostInsertsStartingPosition, sizeof(sycl::float4) * settings.max_inserts * index));
        events.push_back(qt.memcpy(&deviceInsertsMovementPatternIdx[dest_index * settings.max_inserts], hostInsertsMovementPatternIdx, sizeof(int) * settings.max_inserts * index));
        events.push_back(qt.memcpy(&deviceInsertsWords[dest_index * settings.max_inserts], hostInsertsWords, sizeof(int) * settings.max_inserts * index));
        events.push_back(qt.memcpy(&deviceMovements[dest_index * settings.max_movements * settings.max_movement_patterns], hostMovements, sizeof(sycl::float4) * settings.max_movements * settings.max_movement_patterns * index));
        events.push_back(qt.memcpy(&deviceMovementsCounts[dest_index * settings.max_movement_patterns], hostMovementsCounts, sizeof(int) * settings.max_movement_patterns * index));

        sycl::event::wait(events);
    }   
}

void organisation::parallel::inserts::into(::organisation::schema **destination, int destination_size)
{
    int src_client_index = 0;
    int dest_client_index = 0;
    
    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);
    sycl::range num_items{(size_t)settings.clients()};

    do
    {
        std::vector<sycl::event> events;

        events.push_back(qt.memcpy(hostInsertsDelay, &deviceInsertsDelay[src_client_index * settings.max_inserts], sizeof(int) * settings.max_inserts * settings.host_buffer));
        events.push_back(qt.memcpy(hostInsertsLoops, &deviceInsertsLoops[src_client_index * settings.max_inserts], sizeof(int) * settings.max_inserts * settings.host_buffer));
        events.push_back(qt.memcpy(hostInsertsStartingPosition, &deviceInsertsStartingPosition[src_client_index * settings.max_inserts], sizeof(sycl::float4) * settings.max_inserts * settings.host_buffer));
        events.push_back(qt.memcpy(hostInsertsMovementPatternIdx, &deviceInsertsMovementPatternIdx[src_client_index * settings.max_inserts], sizeof(int) * settings.max_inserts * settings.host_buffer));
        events.push_back(qt.memcpy(hostInsertsWords, &deviceInsertsWords[src_client_index * settings.max_inserts], sizeof(int) * settings.max_inserts * settings.host_buffer));
        events.push_back(qt.memcpy(hostMovements, &deviceMovements[src_client_index * settings.max_movements * settings.max_movement_patterns], sizeof(sycl::float4) * settings.max_movements * settings.max_movement_patterns * settings.host_buffer));
        events.push_back(qt.memcpy(hostMovementsCounts, &deviceMovementsCounts[src_client_index * settings.max_movement_patterns], sizeof(int) * settings.max_movement_patterns * settings.host_buffer));

        sycl::event::wait(events);

        for(int i = 0; i < settings.host_buffer; ++i)
        {
            organisation::program *prog = &destination[dest_client_index]->prog;

            for(int j = 0; j < settings.max_inserts; ++j)
            {
                int offset = j + (i * settings.max_inserts);
                int movementPatternIdx = hostInsertsMovementPatternIdx[offset];
                int count = hostMovementsCounts[j + (i * settings.max_movement_patterns)];
                if(count > settings.max_movements) count = settings.max_movements;

                if((movementPatternIdx == -1)||(count == 0)) break;
             
                organisation::genetic::inserts::value value;

                value.delay = hostInsertsDelay[offset];
                value.loops = hostInsertsLoops[offset];
                sycl::float4 starting = hostInsertsStartingPosition[offset];
                value.starting = organisation::point((int)starting.x(), (int)starting.y(), (int)starting.z());
                value.words = hostInsertsWords[offset];
                
                for(int d = 0; d < count; ++d)
                {
                    int offset = (j * settings.max_movements) + d;
                    sycl::float4 movement = hostMovements[(i * settings.max_movements * settings.max_movement_patterns) + offset];
                    vector direction = vector((int)movement.x(), (int)movement.y(), (int)movement.z());

                    value.movement.directions.push_back(direction);
                }
                
                
                prog->insert.values.push_back(value);
            }

            ++dest_client_index;
            if(dest_client_index >= destination_size) return;
        }        

        src_client_index += settings.host_buffer;
    } while((src_client_index * settings.max_inserts) < settings.max_inserts * settings.clients());
}

void organisation::parallel::inserts::outputarb(int *source, int length)
{
	int *temp = new int[length];
	if (temp == NULL) return;

    sycl::queue q = ::parallel::queue(*dev).get();

    q.memcpy(temp, source, sizeof(int) * length).wait();

    std::string result("");
	for (int i = 0; i < length; ++i)
	{
		if ((temp[i] != -1)&&(temp[i]!=0))
		{
			result += std::string("[");
			result += std::to_string(i);
			result += std::string("]");
			result += std::to_string(temp[i]);
			result += std::string(",");
		}
	}
	result += std::string("\r\n");
	
    std::cout << result;

	delete[] temp;
}

void organisation::parallel::inserts::outputarb(sycl::float4 *source, int length)
{
    sycl::float4 *temp = new sycl::float4[length];
    if (temp == NULL) return;

    sycl::queue q = ::parallel::queue(*dev).get();

    q.memcpy(temp, source, sizeof(sycl::float4) * length).wait();

    std::string result("");
	for (int i = 0; i < length; ++i)
	{
        int ix = (int)(temp[i].x() * 100.0f);
        int iy = (int)(temp[i].y() * 100.0f);
        int iz = (int)(temp[i].z() * 100.0f);

        if ((ix != 0) || (iy != 0) || (iz != 0))
        {
			result += std::string("[");
			result += std::to_string(i);
			result += std::string("]");
			result += std::to_string(temp[i].x());
			result += std::string(",");
			result += std::to_string(temp[i].y());
			result += std::string(",");
			result += std::to_string(temp[i].z());
			result += std::string(",");
            result += std::to_string(temp[i].w());
			result += std::string(",");
		}
	}
	result += std::string("\r\n");
	
    
    std::cout << result;

	delete[] temp;
}

void organisation::parallel::inserts::makeNull()
{
    dev = NULL;

    deviceNewPositions = NULL;
    deviceNewValues = NULL;
    deviceNewMovementPatternIdx = NULL;
    deviceNewClient = NULL;
    
    deviceInputData = NULL;
    deviceInsertsDelay = NULL;    
    deviceInsertsLoops = NULL;
    deviceInsertsStartingPosition = NULL;
    deviceInsertsMovementPatternIdx = NULL;
    deviceInsertsWords = NULL;

    deviceMovements = NULL;
    deviceMovementsCounts = NULL;
    
    deviceInputIdx = NULL;

    deviceTotalNewInserts = NULL;

    hostTotalNewInserts = NULL;    
    hostInputData = NULL;

    hostInsertsDelay = NULL;
    hostInsertsLoops = NULL;
    hostInsertsStartingPosition = NULL;
    hostInsertsMovementPatternIdx = NULL;
    hostInsertsWords = NULL;

    hostMovements = NULL;
    hostMovementsCounts = NULL;
}

void organisation::parallel::inserts::cleanup()
{
    if(dev != NULL) 
    {   
        sycl::queue q = ::parallel::queue(*dev).get();

        if(hostMovementsCounts != NULL) sycl::free(hostMovementsCounts, q);
        if(hostMovements != NULL) sycl::free(hostMovements, q);

        if(hostInsertsWords != NULL) sycl::free(hostInsertsWords, q);
        if(hostInsertsMovementPatternIdx != NULL) sycl::free(hostInsertsMovementPatternIdx, q);
        if(hostInsertsStartingPosition != NULL) sycl::free(hostInsertsStartingPosition, q);
        if(hostInsertsLoops != NULL) sycl::free(hostInsertsLoops, q);
        if(hostInsertsDelay != NULL) sycl::free(hostInsertsDelay, q);
        if(hostInputData != NULL) sycl::free(hostInputData, q);
        if(hostTotalNewInserts != NULL) sycl::free(hostTotalNewInserts, q);        
        if(deviceTotalNewInserts != NULL) sycl::free(deviceTotalNewInserts, q);
        if(deviceInputIdx != NULL) sycl::free(deviceInputIdx, q);

        if(deviceMovementsCounts != NULL) sycl::free(deviceMovementsCounts, q);
        if(deviceMovements != NULL) sycl::free(deviceMovements, q);

        if(deviceInsertsWords != NULL) sycl::free(deviceInsertsWords, q);
        if(deviceInsertsMovementPatternIdx != NULL) sycl::free(deviceInsertsMovementPatternIdx, q);
        if(deviceInsertsStartingPosition != NULL) sycl::free(deviceInsertsStartingPosition, q);
        if(deviceInsertsLoops != NULL) sycl::free(deviceInsertsLoops, q);        
        if(deviceInsertsDelay != NULL) sycl::free(deviceInsertsDelay, q);
        if(deviceInputData != NULL) sycl::free(deviceInputData, q);
        if(deviceNewClient != NULL) sycl::free(deviceNewClient, q);
        if(deviceNewMovementPatternIdx != NULL) sycl::free(deviceNewMovementPatternIdx, q);
        if(deviceNewValues != NULL) sycl::free(deviceNewValues, q);
        if(deviceNewPositions != NULL) sycl::free(deviceNewPositions, q);          
    }
}
