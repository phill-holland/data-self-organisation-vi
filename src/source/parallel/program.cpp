#include "parallel/program.hpp"
#include "parallel/parameters.hpp"
#include <algorithm>

int GetCollidedKey(const sycl::float4 a, const sycl::float4 b)
{
    sycl::float4 direction;

    direction.x() = b.x() - a.x(); direction.y() = b.y() - a.y();
    direction.z() = b.z() - a.z();

    if ((direction.x() == 0) && (direction.y() == 0) &&
        (direction.z() == 0)) return 0;

    if (direction.x() < -1.0f) direction.x() = -1.0f;
    if (direction.x() > 1.0f) direction.x() = 1.0f;

    if (direction.y() < -1.0f) direction.y() = -1.0f;
    if (direction.y() > 1.0f) direction.y() = 1.0f;

    if (direction.z() < -1.0f) direction.z() = -1.0f;
    if (direction.z() > 1.0f) direction.z() = 1.0f;

    long x = ((long)(direction.x() + 1.0f));
    long y = ((long)(direction.y() + 1.0f));
    long z = ((long)(direction.z() + 1.0f));

    return (sycl::abs(z) * (3L * 3L)) + (sycl::abs(y) * 3L) + sycl::abs(x);
}

void organisation::parallel::program::reset(::parallel::device &dev, 
                                            ::parallel::queue *q, 
                                            ::parallel::mapper::configuration &mapper,
                                            parameters settings)
{
    init = false; cleanup();

    this->dev = &dev;
    this->queue = q;
    this->settings = settings;
    
    sycl::queue &qt = ::parallel::queue(dev).get();
        
    devicePositions = sycl::malloc_device<sycl::float4>(settings.max_values * settings.clients(), qt);
    if(devicePositions == NULL) return;

    deviceNextPositions = sycl::malloc_device<sycl::float4>(settings.max_values * settings.clients(), qt);
    if(deviceNextPositions == NULL) return;

    deviceNextHalfPositions = sycl::malloc_device<sycl::float4>(settings.max_values * settings.clients(), qt);
    if(deviceNextHalfPositions == NULL) return;

    deviceValues = sycl::malloc_device<sycl::int4>(settings.max_values * settings.clients(), qt);
    if(deviceValues == NULL) return;
    
    deviceNextDirections = sycl::malloc_device<sycl::float4>(settings.max_values * settings.clients(), qt);
    if(deviceNextDirections == NULL) return;

    deviceMovementModifier = sycl::malloc_device<sycl::float4>(settings.max_values * settings.clients(), qt);
    if(deviceMovementModifier == NULL) return;

    deviceMovementIdx = sycl::malloc_device<int>(settings.max_values * settings.clients(), qt);
    if(deviceMovementIdx == NULL) return;

    deviceMovementPatternIdx = sycl::malloc_device<int>(settings.max_values * settings.clients(), qt);
    if(deviceMovementPatternIdx == NULL) return;

    deviceLifetime = sycl::malloc_device<int>(settings.max_values * settings.clients(), qt);
    if(deviceLifetime == NULL) return;

    deviceClient = sycl::malloc_device<sycl::int4>(settings.max_values * settings.clients(), qt);
    if(deviceClient == NULL) return;

    // ***

    deviceCachePositions = sycl::malloc_device<sycl::float4>(settings.max_values * settings.clients(), qt);
    if(deviceCachePositions == NULL) return;

    deviceCacheValues = sycl::malloc_device<sycl::int4>(settings.max_values * settings.clients(), qt);
    if(deviceCacheValues == NULL) return;

    deviceCacheClients = sycl::malloc_device<sycl::int4>(settings.max_values * settings.clients(), qt);
    if(deviceCacheClients == NULL) return;

    deviceCollisionCounts = sycl::malloc_device<int>(settings.epochs() * settings.clients(), qt);
    if(deviceCollisionCounts == NULL) return;

    hostCollisionCounts = sycl::malloc_host<int>(settings.epochs() * settings.clients(), qt);
    if(hostCollisionCounts == NULL) return;

    // ***

    deviceNextCollisionKeys = sycl::malloc_device<sycl::int2>(settings.max_values * settings.clients(), qt);
    if(deviceNextCollisionKeys == NULL) return;

    deviceCurrentCollisionKeys = sycl::malloc_device<sycl::int2>(settings.max_values * settings.clients(), qt);
    if(deviceCurrentCollisionKeys == NULL) return;

    deviceCorrectionCollisionKeys = sycl::malloc_device<sycl::int2>(settings.max_values * settings.clients(), qt);
    if(deviceCorrectionCollisionKeys == NULL) return;

    deviceInsertCollisionKeys = sycl::malloc_device<sycl::int2>(settings.max_values * settings.clients(), qt);
    if(deviceInsertCollisionKeys == NULL) return;

    // ***
    
    hostCachePositions = sycl::malloc_host<sycl::float4>(settings.max_values * settings.host_buffer, qt);
    if(hostCachePositions == NULL) return;

    hostCacheValues = sycl::malloc_host<sycl::int4>(settings.max_values * settings.host_buffer, qt);
    if(hostCacheValues == NULL) return;
    
    hostCacheClients = sycl::malloc_host<sycl::int4>(settings.max_values * settings.host_buffer, qt);
    if(hostCacheClients == NULL) return;

    // ***

    deviceOutputValues = sycl::malloc_device<sycl::int4>(settings.max_values * settings.clients(), qt);
    if(deviceOutputValues == NULL) return;
    
    deviceOutputIndex = sycl::malloc_device<int>(settings.max_values * settings.clients(), qt);
    if(deviceOutputIndex == NULL) return;
    
    deviceOutputClient = sycl::malloc_device<sycl::int4>(settings.max_values * settings.clients(), qt);
    if(deviceOutputClient == NULL) return;

    deviceOutputPosition = sycl::malloc_device<sycl::float4>(settings.max_values * settings.clients(), qt);
    if(deviceOutputPosition == NULL) return;

    deviceOutputTotalValues = sycl::malloc_device<int>(1, qt);
    if(deviceOutputTotalValues == NULL) return;


    hostOutputValues = sycl::malloc_host<sycl::int4>(settings.max_values * settings.clients(), qt);
    if(hostOutputValues == NULL) return;
    
    hostOutputIndex = sycl::malloc_host<int>(settings.max_values * settings.clients(), qt);
    if(hostOutputIndex == NULL) return;
    
    hostOutputClient = sycl::malloc_host<sycl::int4>(settings.max_values * settings.clients(), qt);
    if(hostOutputClient == NULL) return;

    hostOutputPosition = sycl::malloc_host<sycl::float4>(settings.max_values * settings.clients(), qt);
    if(hostOutputPosition == NULL) return;

    hostOutputTotalValues = sycl::malloc_host<int>(1, qt);
    if(hostOutputTotalValues == NULL) return;
    // ***

    deviceTotalValues = sycl::malloc_device<int>(1, qt);
    if(deviceTotalValues == NULL) return;

    hostTotalValues = sycl::malloc_host<int>(1, qt);
    if(hostTotalValues == NULL) return;

    // ***
    deviceNewPositions = sycl::malloc_device<sycl::float4>(settings.max_values * settings.clients(), qt);
    if(deviceNewPositions == NULL) return;

    deviceNewValues = sycl::malloc_device<sycl::int4>(settings.max_values * settings.clients(), qt);
    if(deviceNewValues == NULL) return;

    deviceNewClient = sycl::malloc_device<sycl::int4>(settings.max_values * settings.clients(), qt);
    if(deviceNewClient == NULL) return;

    deviceNewNextDirections = sycl::malloc_device<sycl::float4>(settings.max_values * settings.clients(), qt);
    if(deviceNewNextDirections == NULL) return;

    deviceNewMovementIdx = sycl::malloc_device<int>(settings.max_values * settings.clients(), qt);
    if(deviceNewMovementIdx == NULL) return;

    deviceNewMovementPatternIdx = sycl::malloc_device<int>(settings.max_values * settings.clients(), qt);
    if(deviceNewMovementPatternIdx == NULL) return;

    // ***

    deviceOldPositions = sycl::malloc_device<sycl::float4>(settings.max_values * settings.clients(), qt);
    if(deviceOldPositions == NULL) return;

    deviceOldUpdateCounter = sycl::malloc_device<int>(1, qt);
    if(deviceOldUpdateCounter == NULL) return;

    hostOldUpdateCounter = sycl::malloc_host<int>(1, qt);
    if(hostOldUpdateCounter == NULL) return;

    // ***

    if(settings.history != NULL)
    {
        hostPositions = sycl::malloc_host<sycl::float4>(settings.max_values * settings.clients(), qt);
        if(hostPositions == NULL) return;
    
        hostValues = sycl::malloc_host<sycl::int4>(settings.max_values * settings.clients(), qt);
        if(hostValues == NULL) return;

        hostClient = sycl::malloc_host<sycl::int4>(settings.max_values * settings.clients(), qt);
        if(hostClient == NULL) return;

        hostNextDirections = sycl::malloc_host<sycl::float4>(settings.max_values * settings.clients(), qt);
        if(hostNextDirections == NULL) return;

        hostCollisions = sycl::malloc_host<sycl::int2>(settings.max_values * settings.clients(), qt);
        if(hostCollisions == NULL) return;

        hostMovementIdx = sycl::malloc_host<int>(settings.max_values * settings.clients(), qt);
        if(hostMovementIdx == NULL) return;

        hostMovementPatternIdx = sycl::malloc_host<int>(settings.max_values * settings.clients(), qt);
        if(hostMovementPatternIdx == NULL) return;
    }

    // ***

    ::parallel::parameters global(settings.width * settings.dim_clients.x, settings.height * settings.dim_clients.y, settings.depth * settings.dim_clients.z);
    global.length = settings.max_values * settings.clients();
    ::parallel::parameters client(settings.width, settings.height, settings.depth);
    client.length = settings.max_values;

    impacter = new ::parallel::mapper::map(dev, client, global, mapper);
	if (impacter == NULL) return;	
    if (!impacter->initalised()) return;

    collision = new collisions(dev, q, settings);
    if(collision == NULL) return;
    if(!collision->initalised()) return;

    inserter = new inserts(dev, q, settings);
    if(inserter == NULL) return;
    if(!inserter->initalised()) return;

    linker = new links(dev, q, settings);
    if(linker == NULL) return;
    if(!linker->initalised()) return;

    // ***

    clear();

    init = true;
}

void organisation::parallel::program::clear()
{
    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);

    std::vector<sycl::event> events;

    events.push_back(qt.memset(devicePositions, 0, sizeof(sycl::float4) * settings.max_values * settings.clients()));
    events.push_back(qt.memset(deviceNextPositions, 0, sizeof(sycl::float4) * settings.max_values * settings.clients()));
    events.push_back(qt.memset(deviceNextHalfPositions, 0, sizeof(sycl::float4) * settings.max_values * settings.clients()));
    events.push_back(qt.memset(deviceValues, 0, sizeof(sycl::int4) * settings.max_values * settings.clients()));
    events.push_back(qt.memset(deviceNextDirections, 0, sizeof(sycl::float4) * settings.max_values * settings.clients()));
    events.push_back(qt.memset(deviceMovementModifier, 0, sizeof(sycl::float4) * settings.max_values * settings.clients()));
    events.push_back(qt.memset(deviceMovementIdx, 0, sizeof(int) * settings.max_values * settings.clients()));
    events.push_back(qt.memset(deviceMovementPatternIdx, 0, sizeof(int) * settings.max_values * settings.clients()));
    events.push_back(qt.memset(deviceLifetime, 0, sizeof(int) * settings.max_values * settings.clients()));
    events.push_back(qt.memset(deviceClient, 0, sizeof(sycl::int4) * settings.max_values * settings.clients()));
    
    events.push_back(qt.memset(deviceCachePositions, 0, sizeof(sycl::float4) * settings.max_values * settings.clients()));
    events.push_back(qt.memset(deviceCacheValues, -1, sizeof(sycl::int4) * settings.max_values * settings.clients()));
    events.push_back(qt.memset(deviceCacheClients, 0, sizeof(sycl::int4) * settings.max_values * settings.clients()));
    
    events.push_back(qt.memset(deviceCollisionCounts, 0, sizeof(int) * settings.epochs() * settings.clients()));

    events.push_back(qt.memset(deviceNextCollisionKeys, 0, sizeof(sycl::int2) * settings.max_values * settings.clients()));
    events.push_back(qt.memset(deviceCurrentCollisionKeys, 0, sizeof(sycl::int2) * settings.max_values * settings.clients()));
    events.push_back(qt.memset(deviceCorrectionCollisionKeys, 0, sizeof(sycl::int2) * settings.max_values * settings.clients()));

    collision->clear();
    inserter->clear();
    linker->clear();

    totalOutputValues = 0;
    totalValues = 0;

    outputs.clear();

    sycl::event::wait(events);
}

void organisation::parallel::program::restart()
{
    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);

    std::vector<sycl::event> events1, events2;

    events1.push_back(qt.memset(devicePositions, 0, sizeof(sycl::float4) * settings.max_values * settings.clients()));
    events1.push_back(qt.memset(deviceNextPositions, 0, sizeof(sycl::float4) * settings.max_values * settings.clients()));
    events1.push_back(qt.memset(deviceNextHalfPositions, 0, sizeof(sycl::float4) * settings.max_values * settings.clients()));
    events1.push_back(qt.memset(deviceValues, 0, sizeof(sycl::int4) * settings.max_values * settings.clients()));
    events1.push_back(qt.memset(deviceNextDirections, 0, sizeof(sycl::float4) * settings.max_values * settings.clients()));
    events1.push_back(qt.memset(deviceMovementModifier, 0, sizeof(sycl::float4) * settings.max_values * settings.clients()));
    events1.push_back(qt.memset(deviceMovementIdx, 0, sizeof(int) * settings.max_values * settings.clients()));    
    events1.push_back(qt.memset(deviceLifetime, 0, sizeof(int) * settings.max_values * settings.clients()));
    events1.push_back(qt.memset(deviceClient, 0, sizeof(sycl::int4) * settings.max_values * settings.clients()));
    events1.push_back(qt.memset(deviceTotalValues, 0, sizeof(int)));

    inserter->restart();

    sycl::event::wait(events1);

    sycl::range num_items{(size_t)settings.max_values * settings.clients()};

    qt.submit([&](auto &h) 
    {     
        auto _positions = devicePositions; 
        auto _values = deviceValues;
        auto _client = deviceClient;

        auto _cachePositions = deviceCachePositions; 
        auto _cacheValues = deviceCacheValues;
        auto _cacheClient = deviceCacheClients;

        auto _valuesLength = settings.max_values * settings.clients();
        auto _newTotalValues = deviceTotalValues;

        h.parallel_for(num_items, [=](auto i) 
        {  
            sycl::int4 cacheValue = _cacheValues[i];
            if((cacheValue.x() != -1)||(cacheValue.y() != -1)||(cacheValue.z() != -1))
            {
                cl::sycl::atomic_ref<int, cl::sycl::memory_order::relaxed, 
                            sycl::memory_scope::device, 
                            sycl::access::address_space::ext_intel_global_device_space> ar(_newTotalValues[0]);

                int idx = ar.fetch_add(1);

                if(idx < _valuesLength)
                {
                    _positions[idx] = _cachePositions[i];
                    _values[idx] = cacheValue;
                    _client[idx] = _cacheClient[i];
                }
            }            
        });
    }).wait();

    totalOutputValues = 0;
    qt.memcpy(hostTotalValues, deviceTotalValues, sizeof(int)).wait();
    totalValues = hostTotalValues[0];
  
    if(totalValues > settings.max_values * settings.clients())
            totalValues = settings.max_values * settings.clients();
}

void organisation::parallel::program::run(organisation::data &mappings)
{
    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);
    sycl::range num_items{(size_t)(settings.max_values * settings.clients())};

    outputs.clear();

    if(settings.history != NULL) settings.history->clear();

    for(int epoch = 0; epoch < settings.epochs(); ++epoch)
    {     
        restart();           

        int iterations = 0;
        while(iterations++ < settings.iterations)
        {        
            insert(epoch, iterations);

            qt.memcpy(deviceOldPositions, devicePositions, sizeof(sycl::float4) * totalValues).wait();                            

            positions();

            std::vector<sycl::event> events;
            events.push_back(qt.memset(deviceNextCollisionKeys, 0, sizeof(sycl::int2) * totalValues));
            events.push_back(qt.memset(deviceCurrentCollisionKeys, 0, sizeof(sycl::int2) * totalValues));
            sycl::event::wait(events);            

            // ***

            impacter->build(deviceNextPositions, deviceClient, totalValues, queue);
	        impacter->search(deviceNextPositions, deviceClient, deviceNextCollisionKeys, totalValues, true, false, false, NULL, 0, queue);

            // ***

            impacter->build(deviceNextHalfPositions, deviceClient, totalValues, queue);
            impacter->search(deviceNextHalfPositions, deviceClient, deviceNextCollisionKeys, totalValues, true, false, false, NULL, 0, queue);		

            // ***

            impacter->build(devicePositions, deviceClient, totalValues, queue);
            impacter->search(deviceNextPositions, deviceClient, deviceCurrentCollisionKeys, totalValues, true, false, false, NULL, 0, queue);
            impacter->search(deviceNextHalfPositions, deviceClient, deviceCurrentCollisionKeys, totalValues, true, false, false, NULL, 0, queue);		
            
            // ***
            
            update();
            next();
            corrections();
            outputting(epoch, iterations);
            history(epoch, iterations);
            boundaries();
            
            //stops(iterations);


//std::cout << "positions(" << epoch << "): ";
//outputarb(devicePositions,totalValues);
//std::cout << "nextPos: ";
//outputarb(deviceNextPositions,totalValues);
//std::cout << "nextDir: ";
//outputarb(deviceNextDirections,totalValues);
//std::cout << "client: ";
//outputarb(deviceClient,totalValues);
//std::cout << "values: ";
//outputarb(deviceValues,totalValues);
/*std::cout << "col: ";
outputarb(deviceNextCollisionKeys,totalValues);
std::cout << "outputs: ";
outputarb(deviceOutputValues,totalOutputValues);
std::cout << "patternIdx: ";
outputarb(deviceMovementPatternIdx, totalValues);
std::cout << "movementIdx: ";
outputarb(deviceMovementIdx,totalValues);
std::cout << "counts: ";
outputarb(inserter->deviceMovementsCounts, settings.max_movement_patterns * settings.clients());
std::cout << "modifier: ";
outputarb(deviceMovementModifier, totalValues);
*/
//std::cout << "\r\n";

        };

        move(mappings, epoch);            
        
        qt.memset(deviceTotalValues, 0, sizeof(int)).wait();        
    }    

    if(settings.history != NULL) settings.history->save("data/trace.txt");
}

void organisation::parallel::program::set(organisation::data &mappings, inputs::input &source)
{
    inserter->set(mappings, source);
}

std::vector<organisation::outputs::output> organisation::parallel::program::get(organisation::data &mappings)
{
    return outputs;    
}

void organisation::parallel::program::move(organisation::data &mappings, int epoch)
{    
    if(totalOutputValues > 0)
    {
        sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);

        int output_length = totalOutputValues;
        if(output_length > (settings.max_values * settings.clients())) 
            output_length = (settings.max_values * settings.clients());

        std::vector<sycl::event> events;

        events.push_back(qt.memcpy(hostOutputValues, deviceOutputValues, sizeof(sycl::int4) * output_length));
        events.push_back(qt.memcpy(hostOutputIndex, deviceOutputIndex, sizeof(int) * output_length));
        events.push_back(qt.memcpy(hostOutputClient, deviceOutputClient, sizeof(sycl::int4) * output_length));
        events.push_back(qt.memcpy(hostOutputPosition, deviceOutputPosition, sizeof(sycl::float4) * output_length));

        sycl::event::wait(events);

        if(settings.save_outputs)
        {
            std::string filename ="data/outputs" + std::to_string(epoch) + ".txt";
            std::fstream output(filename, std::fstream::out | std::fstream::binary);
            for(int i = 0; i < output_length; ++i)
            {
                std::string v1 = "V " + std::to_string(hostOutputValues[i].x());
                v1 += "," + std::to_string(hostOutputValues[i].y());
                v1 += "," + std::to_string(hostOutputValues[i].z());
                v1 += " I " + std::to_string(hostOutputIndex[i]);
                v1 += " C(" + std::to_string(hostOutputClient[i].x());
                v1 += "," + std::to_string(hostOutputClient[i].y());
                v1 += "," + std::to_string(hostOutputClient[i].z());
                v1 += "," + std::to_string(hostOutputClient[i].w());
                v1 += ")->ColPos(" + std::to_string(hostOutputPosition[i].x());
                v1 += "," + std::to_string(hostOutputPosition[i].y());
                v1 += "," + std::to_string(hostOutputPosition[i].z());
                v1 += "," + std::to_string(hostOutputPosition[i].w());
                v1 += ")\n";
                output.write(v1.c_str(),v1.size());
            }
            output.close();
        }

        outputs::output out;

        for(int i = 0; i < output_length; ++i)
        {
            outputs::data temp;
            sycl::int4 value = hostOutputValues[i];

            int coordinates[] = { value.x(), value.y(), value.z() };

            for(int j = 0; j < 3; ++j)
            {
                if(coordinates[j] != -1)
                {
                    temp.value = mappings.map(coordinates[j]);
                    temp.client = hostOutputClient[i].w();
                    temp.index = hostOutputIndex[i];

                    out.values.push_back(temp);
                }
            }
        }

        outputs.push_back(out);

        qt.memset(deviceOutputTotalValues, 0, sizeof(int)).wait();
        totalOutputValues = 0;
    }
}

void organisation::parallel::program::update()
{
    if(totalValues == 0) return;

    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);
    sycl::range num_items{(size_t)totalValues};

    qt.submit([&](auto &h) 
    {
        auto _positions = devicePositions;     
        auto _nextDirections = deviceNextDirections;
        auto _collisionKeys = deviceNextCollisionKeys;

        h.parallel_for(num_items, [=](auto i) 
        {  
            if(_positions[i].w() == 0)
            {
                sycl::int2 key = _collisionKeys[i];
                if(key.x() == 0)
                    _positions[i] += _nextDirections[i];
            }
        });
    }).wait();
}

void organisation::parallel::program::positions()
{
    if(totalValues == 0) return;

    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);
    sycl::range num_items{(size_t)totalValues};

    qt.submit([&](auto &h) 
    {    
        auto _position = devicePositions;
        auto _nextPosition = deviceNextPositions;
        auto _nextHalfPosition = deviceNextHalfPositions;
        auto _nextDirection = deviceNextDirections;

        h.parallel_for(num_items, [=](auto i) 
        { 
            _nextPosition[i].x() = _position[i].x() + _nextDirection[i].x();
            _nextPosition[i].y() = _position[i].y() + _nextDirection[i].y();
            _nextPosition[i].z() = _position[i].z() + _nextDirection[i].z();

            _nextHalfPosition[i].x() = sycl::round(
                _position[i].x() + (_nextDirection[i].x() / 2.0f));
            _nextHalfPosition[i].y() = sycl::round(
                _position[i].y() + (_nextDirection[i].y() / 2.0f));
            _nextHalfPosition[i].z() = sycl::round(
                _position[i].z() + (_nextDirection[i].z() / 2.0f));
        });
    }).wait();        
}

void organisation::parallel::program::next()
{
    if(totalValues == 0) return;

    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);
    sycl::range num_items{(size_t)totalValues};

    qt.submit([&](auto &h) 
    {
        auto _values = deviceValues;
        auto _positions = devicePositions;   
        auto _nextPositions = deviceNextPositions; 
        auto _movementModifier = deviceMovementModifier;
        auto _client = deviceClient;
        auto _movementIdx = deviceMovementIdx;
        auto _movementPatternIdx = deviceMovementPatternIdx;
        auto _movements = inserter->deviceMovements;
        auto _movementsCounts = inserter->deviceMovementsCounts;
        auto _collisions = collision->deviceCollisions;
        auto _nextDirections = deviceNextDirections;
        auto _collisionKeys = deviceNextCollisionKeys;

        auto _max_movements = settings.max_movements;
        auto _max_collisions = settings.max_collisions;
        auto _max_movement_patterns = settings.max_movement_patterns;
        auto _max_words = settings.mappings.maximum();

        h.parallel_for(num_items, [=](auto i) 
        {  
            if(_positions[i].w() == 0)
            {            
                int client = _client[i].w();                

                sycl::int2 collision = _collisionKeys[i];
                if(collision.x() > 0)
                {
                    if (_collisionKeys[collision.y()].x() > 0 && _positions[collision.y()].w() != -2)
                    {
                        int key1 = GetCollidedKey(_positions[i], _nextPositions[i]);
                        int offset1 = (client * _max_collisions * _max_words) + (_max_collisions * _values[i].x()) + key1;
                        sycl::float4 direction1 = _collisions[offset1];

                        int coordinates1[] = { _values[i].y(), _values[i].z() };
                        int wordIdx1 = 0;
                        while((coordinates1[wordIdx1] != -1)&&(wordIdx1<2))
                        {
                            if(coordinates1[wordIdx1] != -1)
                            {
                                int tempOffset1 = (client * _max_collisions * _max_words) + (_max_collisions * coordinates1[wordIdx1]) + key1;
                                sycl::float4 tempDirection1 = _collisions[tempOffset1];

                                sycl::float4 new_direction = { 
                                direction1.y() * tempDirection1.z() - direction1.z() * tempDirection1.y(),
                                direction1.z() * tempDirection1.x() - direction1.x() * tempDirection1.z(),
                                direction1.x() * tempDirection1.y() - direction1.y() * tempDirection1.x(),
                                0.0f };                    

                                direction1 = tempDirection1;
                            }
                            wordIdx1++;
                        };
                        // ***

                        int key2 = GetCollidedKey(_positions[collision.y()], _nextPositions[collision.y()]);
                        int offset2 = (client * _max_collisions * _max_words) + (_max_collisions * _values[collision.y()].x()) + key2;
                        sycl::float4 direction2 = _collisions[offset2];

                        int coordinates2[] = { _values[collision.y()].y(), _values[collision.y()].z() };
                        int wordIdx2 = 0;
                        while((coordinates2[wordIdx2] != -1)&&(wordIdx2<2))
                        {
                            if(coordinates2[wordIdx2] != -1)
                            {
                                int tempOffset1 = (client * _max_collisions * _max_words) + (_max_collisions * coordinates2[wordIdx2]) + key2;
                                sycl::float4 tempDirection1 = _collisions[tempOffset1];

                                sycl::float4 new_direction = { 
                                direction2.y() * tempDirection1.z() - direction2.z() * tempDirection1.y(),
                                direction2.z() * tempDirection1.x() - direction2.x() * tempDirection1.z(),
                                direction2.x() * tempDirection1.y() - direction2.y() * tempDirection1.x(),
                                0.0f };                    

                                direction2 = tempDirection1;
                            }

                            wordIdx2++;
                        };

                        sycl::float4 new_direction = { 
                            direction1.y() * direction2.z() - direction1.z() * direction2.y(),
                            direction1.z() * direction2.x() - direction1.x() * direction2.z(),
                            direction1.x() * direction2.y() - direction1.y() * direction2.x(),
                            0.0f };                    

                        new_direction.x() = sycl::clamp(new_direction.x(),-1.0f,1.0f);
                        new_direction.y() = sycl::clamp(new_direction.y(),-1.0f,1.0f);
                        new_direction.z() = sycl::clamp(new_direction.z(),-1.0f,1.0f);

                        _nextDirections[i] = new_direction;    
                        // ***
                        _movementModifier[i] = { new_direction.x(), new_direction.y(), new_direction.z(), 1.0f };
                        // ****

                        //int key1 = GetCollidedKey(_positions[i], _nextPositions[i]);
                        //int offset1 = (client * _max_collisions * _max_words) + (_max_collisions * _values[i].x()) + key1;
                        //sycl::float4 direction1 = _collisions[offset1];
                 /*
                        int key2 = GetCollidedKey(_positions[collision.y()], _nextPositions[collision.y()]);
                        int offset2 = (client * _max_collisions * _max_words) + (_max_collisions * _values[collision.y()].x()) + key2;
                        
                        sycl::float4 direction2 = _collisions[offset2];

                        sycl::float4 new_direction = { 
                            direction1.y() * direction2.z() - direction1.z() * direction2.y(),
                            direction1.z() * direction2.x() - direction1.x() * direction2.z(),
                            direction1.x() * direction2.y() - direction1.y() * direction2.x(),
                            0.0f };                    

                        _nextDirections[i] = new_direction;                    
                        */
                    }
                    else
                    {
                        // technically, values[i] can have 3 values in them, x,y,z!
                        // can insert three values at once too, technically (not implemented yet)
                        /*
                        int key1 = GetCollidedKey(_positions[i], _nextPositions[i]);
                        int offset1 = (client * _max_collisions * _max_words) + (_max_collisions * _values[i].x()) + key1;

                        int key2 = GetCollidedKey(_nextPositions[i], _positions[i]);
                        int offset2 = (client * _max_collisions * _max_words) + (_max_collisions * _values[collision.y()].x()) + key2;

                        sycl::float4 direction1 = _collisions[offset1];
                        sycl::float4 direction2 = _collisions[offset2];

                        sycl::float4 new_direction = { 
                            direction1.y() * direction2.z() - direction1.z() * direction2.y(),
                            direction1.z() * direction2.x() - direction1.x() * direction2.z(),
                            direction1.x() * direction2.y() - direction1.y() * direction2.x(),
                            0.0f };                    

                        _nextDirections[i] = new_direction;                    
                        */
                        
                        int key1 = GetCollidedKey(_positions[i], _nextPositions[i]);                        
                        int offset1 = (client * _max_collisions * _max_words) + (_max_collisions * _values[i].x()) + key1;
                        sycl::float4 direction1 = _collisions[offset1];
                        _nextDirections[i] = direction1;
                        // ***
                        _movementModifier[i] = { direction1.x(), direction1.y(), direction1.z(), 1.0f };  
                        // ***                        
                    }
                }
                else
                {
                    int a = _movementIdx[i];
                    int offset = _max_movements * _max_movement_patterns * client;
                    int movement_pattern_idx = _movementPatternIdx[i];

                    sycl::float4 direction = _movements[a + offset + (movement_pattern_idx * _max_movements)];

                    // ***
                    /*
                    if(_movementModifier[i].w() >= 1.0f)
                    {                        
                        sycl::float4 modifier = _movementModifier[i];
                        direction = { 
                            direction.y() * modifier.z() - direction.z() * modifier.y(),
                            direction.z() * modifier.x() - direction.x() * modifier.z(),
                            direction.x() * modifier.y() - direction.y() * modifier.x(),
                            0.0f };                       

                            //out << "here " << modifier.x() << "," << modifier.y() << "," << modifier.z() << " " << direction.x() << "," << direction.y() << "," << direction.z() << "\n";
                    }
                    */
                    // ***
                    _nextDirections[i] = direction;            

                    _movementIdx[i]++;      
                    int temp = _movementIdx[i];          
                    
                    if((temp >= _max_movements)||(temp >= _movementsCounts[(client * _max_movement_patterns) + movement_pattern_idx]))
                        _movementIdx[i] = 0;            
                }
            }
        });
    }).wait();
}

void organisation::parallel::program::insert(int epoch, int iteration)
{
    int count = inserter->insert(epoch, iteration);

    if(count + totalValues >= settings.max_values * settings.clients())
        count = (count + totalValues) - (settings.max_values * settings.clients());

    if(count > 0)
    {
        sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);
        sycl::range num_items{(size_t)count};

        std::vector<sycl::event> events;

        events.push_back(qt.memset(deviceNextCollisionKeys, 0, sizeof(sycl::int2) * count));
        events.push_back(qt.memset(deviceInsertCollisionKeys, 0, sizeof(sycl::int2) * count));

        sycl::event::wait(events);

        impacter->build(devicePositions, deviceClient, totalValues, queue);
        impacter->search(inserter->deviceNewPositions, inserter->deviceNewClient, deviceNextCollisionKeys, count, false, false, false, NULL, 0, queue);

        impacter->build(inserter->deviceNewPositions, inserter->deviceNewClient, count, queue);
        impacter->search(inserter->deviceNewPositions, inserter->deviceNewClient, deviceInsertCollisionKeys, count, true, true, false, NULL, 0, queue);

        hostTotalValues[0] = totalValues;
        qt.memcpy(deviceTotalValues, hostTotalValues, sizeof(int)).wait();

        qt.submit([&](auto &h) 
        {        
            auto _positions = devicePositions; 
            auto _values = deviceValues;
            auto _lifetime = deviceLifetime;
            auto _movementPatternIdx = deviceMovementPatternIdx;
            auto _movementIdx = deviceMovementIdx;
            auto _movementModifier = deviceMovementModifier;
            auto _client = deviceClient;
            
            auto _srcPosition = inserter->deviceNewPositions;
            auto _srcValues = inserter->deviceNewValues;
            auto _srcMovementPatternIdx = inserter->deviceNewMovementPatternIdx;            
            auto _srcClient = inserter->deviceNewClient;
            
            auto _insertKeys = deviceNextCollisionKeys;
            auto _startingKeys = deviceInsertCollisionKeys;

            auto _valuesLength = settings.max_values * settings.clients();

            auto _iteration = iteration;

            auto _totalValues = deviceTotalValues;

            auto _movements = inserter->deviceMovements;
            auto _nextDirections = deviceNextDirections;
            auto _max_movements = settings.max_movements;
            auto _max_movement_patterns = settings.max_movement_patterns;

            h.parallel_for(num_items, [=](auto i) 
            {  
                if((_insertKeys[i].x() == 0)&&(_startingKeys[i].x() == 0))
                {                
                    cl::sycl::atomic_ref<int, cl::sycl::memory_order::relaxed, 
                                sycl::memory_scope::device, 
                                sycl::access::address_space::ext_intel_global_device_space> ar(_totalValues[0]);

                    int idx = ar.fetch_add(1);               

                    if(idx < _valuesLength)
                    {
                        int movement_pattern_idx = _srcMovementPatternIdx[i];

                        _positions[idx] = _srcPosition[i];
                        _values[idx] = _srcValues[i];
                        _lifetime[idx] = _iteration;
                        _movementPatternIdx[idx] = movement_pattern_idx;
                        _movementIdx[idx] = 0;
                        _movementModifier[idx] = { 0.0f, 0.0f, 0.0f, 0.0f };
                        _client[idx] = _srcClient[i];

                        int offset = _max_movements * _max_movement_patterns * _srcClient[i].w();                
                        sycl::float4 direction = _movements[offset + (movement_pattern_idx * _max_movements)];

                        _nextDirections[idx] = direction;            
                    }
                }
            });
        }).wait();
        
        qt.memcpy(hostTotalValues, deviceTotalValues, sizeof(int)).wait();
            
        totalValues = hostTotalValues[0];

        if(totalValues > settings.max_values * settings.clients())
            totalValues = settings.max_values * settings.clients();
    }
}

void organisation::parallel::program::stops(int iteration)
{
    if(totalValues == 0) return;

    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);
    sycl::range num_items{(size_t)totalValues};

        qt.submit([&](auto &h) 
    {
        auto _positions = devicePositions;   
        auto _lifetimes = deviceLifetime;
        auto _client = deviceClient;
        auto _movementPatternIdx = deviceMovementPatternIdx;
        auto _loops = inserter->deviceInsertsLoops;

        auto _max_inserts = settings.max_inserts;
        auto _iteration = iteration;

        h.parallel_for(num_items, [=](auto i) 
        {  
            if(_positions[i].w() == 0)
            {
                int client = _client[i].w();
                int offset = (client * _max_inserts);
                int a = _movementPatternIdx[offset + i];
                int l = _loops[offset + a];

                if((_iteration - _lifetimes[i]) > l)
                {
                    _positions[i].w() = -2;
                }
            }
        });
    }).wait();
}

void organisation::parallel::program::boundaries()
{
    if(totalValues == 0) return;

    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);
    sycl::range num_items{(size_t)totalValues};

    qt.memset(deviceTotalValues, 0, sizeof(int)).wait();

    qt.submit([&](auto &h) 
    {        
        auto _positions = devicePositions; 
        auto _values = deviceValues;
        auto _client = deviceClient;
        auto _nextDirection = deviceNextDirections;
        auto _movementIdx = deviceMovementIdx;
        auto _movementPatternIdx = deviceMovementPatternIdx;

        auto _newPositions = deviceNewPositions;
        auto _newValues = deviceNewValues;
        auto _newClient = deviceNewClient;
        auto _newNextDirection = deviceNewNextDirections;
        auto _newMovementIdx = deviceNewMovementIdx;    
        auto _newMovementPatternIdx = deviceNewMovementPatternIdx;
        
        auto _newTotalValues = deviceTotalValues;

        auto _width = settings.width;
        auto _height = settings.height;
        auto _depth = settings.depth;

        h.parallel_for(num_items, [=](auto i) 
        {  
            sycl::float4 temp = _positions[i];
            if((temp.x() >= 0)&&(temp.x() < _width)&&
                (temp.y() >= 0)&&(temp.y() < _height)&&
                (temp.z() >= 0)&&(temp.z() < _depth))
            {
                cl::sycl::atomic_ref<int, cl::sycl::memory_order::relaxed, 
                            sycl::memory_scope::device, 
                            sycl::access::address_space::ext_intel_global_device_space> ar(_newTotalValues[0]);

                int idx = ar.fetch_add(1);

                _newPositions[idx] = temp;
                _newValues[idx] = _values[i];
                _newClient[idx] = _client[i];
                _newNextDirection[idx] = _nextDirection[i];
                _newMovementIdx[idx] = _movementIdx[i];
                _newMovementPatternIdx[idx] = _movementPatternIdx[i];
            }
        });
    }).wait();

    qt.memcpy(hostTotalValues, deviceTotalValues, sizeof(int)).wait();
    int temp = hostTotalValues[0];

    if(temp != totalValues)
    {
        if(temp > 0)
        {
            std::vector<sycl::event> events;

            events.push_back(qt.memcpy(devicePositions, deviceNewPositions, sizeof(sycl::float4) * temp));
            events.push_back(qt.memcpy(deviceValues, deviceNewValues, sizeof(sycl::int4) * temp));
            events.push_back(qt.memcpy(deviceClient, deviceNewClient, sizeof(sycl::int4) * temp));
            events.push_back(qt.memcpy(deviceNextDirections, deviceNewNextDirections, sizeof(sycl::float4) * temp));
            events.push_back(qt.memcpy(deviceMovementIdx, deviceNewMovementIdx, sizeof(int) * temp));
            events.push_back(qt.memcpy(deviceMovementPatternIdx, deviceNewMovementPatternIdx, sizeof(int) * temp));

            sycl::event::wait(events);
        }

        totalValues = temp;
    }
}

void organisation::parallel::program::corrections(bool debug)
{
    if(totalValues == 0) return;

    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);
    sycl::range num_items{(size_t)totalValues};

    int counter = 0;

    do
    {
        qt.memset(deviceCorrectionCollisionKeys, 0, sizeof(sycl::int2) * totalValues).wait();
        qt.memset(deviceOldUpdateCounter, 0, sizeof(int)).wait();

        impacter->build(devicePositions, deviceClient, totalValues, queue);
        impacter->search(devicePositions, deviceClient, deviceCorrectionCollisionKeys, totalValues, true, false, false, NULL, 0, queue);
            
        qt.submit([&](auto &h) 
        {      
            auto _positions = devicePositions;     
            auto _oldPositions = deviceOldPositions;
            auto _collisionKeys = deviceCorrectionCollisionKeys;

            auto _updateCounter = deviceOldUpdateCounter;

            h.parallel_for(num_items, [=](auto i) 
            {    
                if(_collisionKeys[i].x() > 0)
                {
                    _positions[i] = _oldPositions[i];
                    cl::sycl::atomic_ref<int, cl::sycl::memory_order::relaxed, 
                            sycl::memory_scope::device, 
                            sycl::access::address_space::ext_intel_global_device_space> ar(_updateCounter[0]);

                    ar.fetch_add(1);
                }
            });
        }).wait();

        qt.memcpy(hostOldUpdateCounter, deviceOldUpdateCounter, sizeof(int)).wait();
        counter = hostOldUpdateCounter[0];
        
        if((counter > 0)&&(debug))
        {
            std::cout << "COUNTER " << counter << "\r\n";

            std::cout << "positions ";
            outputarb(devicePositions,totalValues);
            std::cout << "clients ";
            outputarb(deviceClient,totalValues);
            std::cout << "collision keys ";
            outputarb(deviceCorrectionCollisionKeys, totalValues);
        }   
            
    } while(counter > 0);
}

void organisation::parallel::program::outputting(int epoch, int iteration)
{
    if(totalValues == 0) return;

    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);
    sycl::range num_items{(size_t)totalValues};

    hostOutputTotalValues[0] = totalOutputValues;
    qt.memcpy(deviceOutputTotalValues, hostOutputTotalValues, sizeof(int)).wait();

    qt.submit([&](auto &h) 
    {
        auto _values = deviceValues;
        auto _positions = devicePositions;           
        auto _oldPositions = deviceOldPositions;
        auto _collisionCounts = deviceCollisionCounts;
        auto _client = deviceClient;        
        auto _nextCollisionKeys = deviceNextCollisionKeys;
        auto _currentCollisionKeys = deviceCurrentCollisionKeys;

        auto _dataLinks = linker->deviceLinks;

        auto _outputValues = deviceOutputValues;
        auto _outputIndex = deviceOutputIndex;
        auto _outputClient = deviceOutputClient;
        auto _outputPosition = deviceOutputPosition;
        auto _outputTotalValues = deviceOutputTotalValues;

        auto _outputLength = settings.max_values * settings.clients();

        auto _iteration = iteration;
        auto _epoch = epoch;

        auto _max_hash = settings.mappings.maximum();
        auto _max_chain = settings.max_chain;

        auto _clients = settings.clients();

        auto _outputStationaryOnly = settings.output_stationary_only;

        h.parallel_for(num_items, [=](auto i) 
        {  
            if(_positions[i].w() == 0)
            {   
                cl::sycl::atomic_ref<int, cl::sycl::memory_order::relaxed, 
                sycl::memory_scope::device, 
                sycl::access::address_space::ext_intel_global_device_space> ac(_collisionCounts[(_epoch * _clients) + _client[i].w()]);

                bool output = false, collision = false;
                sycl::int4 value = { -1, -1, -1, -1 };
                sycl::float4 pos = { 0, 0, 0, 0 };

                if((((int)_positions[i].x()) == ((int)_oldPositions[i].x()))
                    &&(((int)_positions[i].y()) == ((int)_oldPositions[i].y()))
                    &&(((int)_positions[i].z()) == ((int)_oldPositions[i].z())))
                {
                    sycl::int2 currentCollision = _currentCollisionKeys[i];
                    if(currentCollision.x() > 0) 
                    {                 
                        if((_positions[currentCollision.y()].w() == -2)||(!_outputStationaryOnly))
                        {   
                            value = _values[currentCollision.y()];
                            pos = _positions[currentCollision.y()];
                            output = true;
                        }

                        if(_positions[currentCollision.y()].w() != -2)         
                            collision = true;
                    }
                }

                sycl::int2 nextCollision = _nextCollisionKeys[i];
                if(nextCollision.x() > 0) 
                {
                    if((_positions[nextCollision.y()].w() == -2)||(!_outputStationaryOnly))
                    {   
                        value = _values[nextCollision.y()];
                        pos = _positions[nextCollision.y()];
                        output = true;
                    }

                    if(_positions[nextCollision.y()].w() != -2)
                        collision = true;
                }

                if(collision) 
                    ac.fetch_add(1);

                if(output)
                {
                    int coordinates1[] = { value.x(), value.y(), value.z() };
                    for(int x = 0; x < 3; ++x)
                    {
                        if(coordinates1[x] != -1)
                        {
                            int chain_idx = _max_hash * _max_chain * _client[i].w();
                            chain_idx += coordinates1[x] * _max_chain;

                            for(int y = 0; y < _max_chain; ++y)
                            {
                                sycl::int4 v1 = _dataLinks[chain_idx + y];
                                if(!((v1.x() == -1)&&(v1.y() == -1)&&(v1.z() == -1)))
                                {
                                    cl::sycl::atomic_ref<int, cl::sycl::memory_order::relaxed, 
                                    sycl::memory_scope::device, 
                                    sycl::access::address_space::ext_intel_global_device_space> ar(_outputTotalValues[0]);

                                    int idx = ar.fetch_add(1);

                                    if(idx < _outputLength)
                                    {                    
                                        _outputValues[idx] = v1;
                                        _outputIndex[idx] = _iteration;
                                        _outputClient[idx] = _client[i];   
                                        _outputPosition[idx] = pos;                     
                                    }
                                }
                            }
                        }
                    }
                }  
            }
        });
    }).wait();

    qt.memcpy(hostOutputTotalValues, deviceOutputTotalValues, sizeof(int)).wait();
    totalOutputValues = hostOutputTotalValues[0];
}

void organisation::parallel::program::history(int epoch, int iteration)
{
    if(settings.history != NULL)
    {
        if(totalValues > 0)
        {
            sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);
            
            std::vector<sycl::event> events;

            events.push_back(qt.memcpy(hostPositions, devicePositions, sizeof(sycl::float4) * totalValues));
            events.push_back(qt.memcpy(hostValues, deviceValues, sizeof(sycl::int4) * totalValues));
            events.push_back(qt.memcpy(hostClient, deviceClient, sizeof(sycl::int4) * totalValues));
            events.push_back(qt.memcpy(hostNextDirections, deviceNextDirections, sizeof(sycl::float4) * totalValues));
            events.push_back(qt.memcpy(hostCollisions, deviceNextCollisionKeys, sizeof(sycl::int2) * totalValues));

            events.push_back(qt.memcpy(hostMovementIdx, deviceMovementIdx, sizeof(int) * totalValues));
            events.push_back(qt.memcpy(hostMovementPatternIdx, deviceMovementPatternIdx, sizeof(int) * totalValues));

            sycl::event::wait(events);

            for(int i = 0; i < totalValues; ++i)
            {
                history::value temp;

                temp.position = point((int)hostPositions[i].x(), (int)hostPositions[i].y(), (int)hostPositions[i].z());
                temp.data = point(hostValues[i].x(), hostValues[i].y(), hostValues[i].z());
                temp.next = point(hostNextDirections[i].x(), hostNextDirections[i].y(), hostNextDirections[i].z());
                temp.sequence = iteration;
                temp.client = hostClient[i].w();
                temp.epoch = epoch;
                temp.stationary = hostPositions[i].w() == -2;
                temp.movementIdx = hostMovementIdx[i];
                temp.movementPatternIdx = hostMovementPatternIdx[i];

                if(hostCollisions[i].x() == 1)
                {
                    sycl::float4 collision = hostPositions[hostCollisions[i].y()];
                    temp.collision = point((int)collision.x(),(int)collision.y(),(int)collision.z());
                    temp.colType = (int)collision.w();
                }

                settings.history->push_back(temp);
            }
        }
    }
}

std::vector<organisation::parallel::value> organisation::parallel::program::get()
{
    std::vector<value> result;

    int totals = totalValues;
    if(totals > 0)
    {
        std::vector<sycl::int4> values = dev->get(deviceValues, totals);
        std::vector<sycl::int4> clients = dev->get(deviceClient, totals);
        std::vector<int> lifetimes = dev->get(deviceLifetime, totals);
        std::vector<sycl::float4> positions = dev->get(devicePositions, totals);

        if((values.size() == totals)&&(clients.size() == totals)&&(positions.size() == totals)&&(lifetimes.size() == totals))
        {
            int len = values.size();
            for(int i = 0; i < len; ++i)
            {
                if(positions[i].w() != -2)
                {
                    value temp;

                    temp.data = point(values[i].x(), values[i].y(), values[i].z());
                    temp.client = clients[i].w();
                    temp.lifetime = lifetimes[i];
                    temp.position = point(positions[i].x(), positions[i].y(), positions[i].z());

                    result.push_back(temp);
                }
            }
        }
    }

    return result;
}

std::vector<organisation::statistics::statistic> organisation::parallel::program::statistics()
{
    std::vector<organisation::statistics::statistic> result(settings.clients());

    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);
    
    qt.memcpy(hostCollisionCounts, deviceCollisionCounts, sizeof(int) * settings.epochs() * settings.clients()).wait();

    for(int epoch = 0; epoch < settings.epochs(); ++epoch)
    {
        int offset = epoch * settings.clients();
        for(int client = 0; client < settings.clients(); ++client)
        {                
            organisation::statistics::data data(hostCollisionCounts[offset + client]);
            result[client].epochs[epoch] = data; 
        }
    }

    return result;
}

void organisation::parallel::program::copy(::organisation::schema **source, int source_size)
{
    auto map = [](const int index, organisation::point dimensions)
    {
        int n = dimensions.x * dimensions.y;
        int r = index % n;
        float z = (float)((index / n));

        int j = r % dimensions.x;
        float y = (float)((r / dimensions.x));
        float x = (float)(j);

        return sycl::int4(x, y, z, index);
    };

    memset(hostCachePositions, 0, sizeof(sycl::float4) * settings.max_values * settings.host_buffer);
    memset(hostCacheValues, -1, sizeof(sycl::int4) * settings.max_values * settings.host_buffer);
    memset(hostCacheClients, 0, sizeof(sycl::int4) * settings.max_values * settings.host_buffer);
    
    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);
    sycl::range num_items{(size_t)settings.clients()};

    int client_index = 0;
    int dest_index = 0;
    int index = 0;

    for(int source_index = 0; source_index < source_size; ++source_index)
    {
        organisation::program *prog = &source[source_index]->prog;

        int d_count = 0;
        for(auto &it: prog->caches.values)
        {
            point value = std::get<0>(it);
            point position = std::get<1>(it);

            hostCachePositions[d_count + (index * settings.max_values)] = { (float)position.x, (float)position.y, (float)position.z, -2.0f };
            hostCacheValues[d_count + (index * settings.max_values)] = { value.x, value.y, value.z, -1 };

            auto m = map(client_index, settings.dim_clients);
            hostCacheClients[d_count + (index * settings.max_values)] = m;

            ++d_count;
            if(d_count >= settings.max_values) break;
        }

        ++index;
        ++client_index;

        if(index >= settings.host_buffer)
        {
            std::vector<sycl::event> events;

            events.push_back(qt.memcpy(&deviceCachePositions[dest_index * settings.max_values], hostCachePositions, sizeof(sycl::float4) * settings.max_values * index));
            events.push_back(qt.memcpy(&deviceCacheValues[dest_index * settings.max_values], hostCacheValues, sizeof(sycl::int4) * settings.max_values * index));
            events.push_back(qt.memcpy(&deviceCacheClients[dest_index * settings.max_values], hostCacheClients, sizeof(sycl::int4) * settings.max_values * index));
            
            sycl::event::wait(events);
            
            memset(hostCachePositions, 0, sizeof(sycl::float4) * settings.max_values * settings.host_buffer);
            memset(hostCacheValues, -1, sizeof(sycl::int4) * settings.max_values * settings.host_buffer);
            memset(hostCacheClients, 0, sizeof(sycl::int4) * settings.max_values * settings.host_buffer);
                                    
            dest_index += settings.host_buffer;
            index = 0;            
        }
    }

    if(index > 0)
    {
        std::vector<sycl::event> events;

        events.push_back(qt.memcpy(&deviceCachePositions[dest_index * settings.max_values], hostCachePositions, sizeof(sycl::float4) * settings.max_values * index));
        events.push_back(qt.memcpy(&deviceCacheValues[dest_index * settings.max_values], hostCacheValues, sizeof(sycl::int4) * settings.max_values * index));
        events.push_back(qt.memcpy(&deviceCacheClients[dest_index * settings.max_values], hostCacheClients, sizeof(sycl::int4) * settings.max_values * index));
        
        sycl::event::wait(events);
    }    

    collision->copy(source, source_size);
    inserter->copy(source, source_size);
    linker->copy(source, source_size);
}

void organisation::parallel::program::into(::organisation::schema **destination, int destination_size)
{
    int src_client_index = 0;
    int dest_client_index = 0;
    
    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);
    sycl::range num_items{(size_t)settings.clients()};

    do
    {
        std::vector<sycl::event> events;

        events.push_back(qt.memcpy(hostCachePositions, &deviceCachePositions[src_client_index * settings.max_values], sizeof(sycl::float4) * settings.max_values * settings.host_buffer));
        events.push_back(qt.memcpy(hostCacheValues, &deviceCacheValues[src_client_index * settings.max_values], sizeof(sycl::int4) * settings.max_values * settings.host_buffer));
        
        sycl::event::wait(events);

        for(int i = 0; i < settings.host_buffer; ++i)
        {
            organisation::program *prog = &destination[dest_client_index]->prog;

            for(int j = 0; j < settings.max_values; ++j)
            {
                sycl::float4 position = hostCachePositions[j + (i * settings.max_values)];
                sycl::int4 value = hostCacheValues[j + (i * settings.max_values)];

                if(value.x() != -1)
                    prog->caches.set(point(value.x(),value.y(),value.z()),point((int)position.x(),(int)position.y(),(int)position.z()));
            }

            ++dest_client_index;
            if(dest_client_index >= destination_size) break;
        }        

        src_client_index += settings.host_buffer;
    } while((src_client_index * settings.max_values) < settings.max_values * settings.clients());

    collision->into(destination, destination_size);
    inserter->into(destination, destination_size);
}

void organisation::parallel::program::debug()
{
    std::cout << "movementsIdx ";
    outputarb(deviceMovementIdx,totalValues);
    std::cout << "positions ";
    outputarb(devicePositions,totalValues);
    std::cout << "next positions ";
    outputarb(deviceNextPositions,totalValues);
    std::cout << "next half positions ";
    outputarb(deviceNextHalfPositions,totalValues);
    std::cout << "next directions ";
    outputarb(deviceNextDirections, totalValues);
    std::cout << "values ";
    outputarb(deviceValues,totalValues);
    std::cout << "clients ";
    outputarb(deviceClient,totalValues);
    std::cout << "deviceInserts ";
    outputarb(inserter->deviceInsertsDelay, settings.max_inserts * settings.clients());
    std::cout << "InputIdx ";
    outputarb(inserter->deviceInputIdx, settings.clients());
    std::cout << "collision keys A";
    outputarb(deviceNextCollisionKeys, totalValues);
    std::cout << "collision keys B";
    outputarb(deviceNextCollisionKeys, totalValues);    
    std::cout << "cache pos ";
    outputarb(deviceCachePositions, settings.max_values * settings.clients());
    std::cout << "cache values ";
    outputarb(deviceCacheValues, settings.max_values * settings.clients());
    std::cout << "cache client ";
    outputarb(deviceCacheClients, settings.max_values * settings.clients());
}

void organisation::parallel::program::outputarb(int *source, int length)
{
	int *temp = new int[length];
	if (temp == NULL) return;

    sycl::queue q = ::parallel::queue(*dev).get();

    q.memcpy(temp, source, sizeof(int) * length).wait();

    std::string result("");
	for (int i = 0; i < length; ++i)
	{
        result += std::string("[");
        result += std::to_string(i);
        result += std::string("]");
        result += std::to_string(temp[i]);
        result += std::string(",");
	}
	result += std::string("\r\n");
	
    std::cout << result;

	delete[] temp;
}

void organisation::parallel::program::outputarb(sycl::int2 *source, int length)
{
    sycl::int2 *temp = new sycl::int2[length];
    if (temp == NULL) return;

    sycl::queue q = ::parallel::queue(*dev).get();

    q.memcpy(temp, source, sizeof(sycl::int2) * length).wait();

    std::string result("");
	for (int i = 0; i < length; ++i)
	{        
        if ((temp[i].x() != 0) || (temp[i].y() != 0))
        {
			result += std::string("[");
			result += std::to_string(i);
			result += std::string("]");
			result += std::to_string(temp[i].x());
			result += std::string(",");
			result += std::to_string(temp[i].y());
		}
	}
	result += std::string("\r\n");
	
    
    std::cout << result;

	delete[] temp;
}

void organisation::parallel::program::outputarb(sycl::int4 *source, int length)
{
    sycl::int4 *temp = new sycl::int4[length];
    if (temp == NULL) return;

    sycl::queue q = ::parallel::queue(*dev).get();

    q.memcpy(temp, source, sizeof(sycl::int4) * length).wait();

    std::string result("");
	for (int i = 0; i < length; ++i)
	{
        if ((temp[i].x() != 0) || (temp[i].y() != 0) || (temp[i].z() != 0))
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

void organisation::parallel::program::outputarb(sycl::float4 *source, int length)
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

void organisation::parallel::program::makeNull()
{
    dev = NULL;

    devicePositions = NULL;
    deviceNextPositions = NULL;
    deviceNextHalfPositions = NULL;
    deviceValues = NULL;
    deviceNextDirections = NULL;
    deviceMovementModifier = NULL;
    
    deviceMovementIdx = NULL;
    deviceMovementPatternIdx = NULL;
    deviceLifetime = NULL;
    deviceClient = NULL;

    deviceCachePositions = NULL;
    deviceCacheValues = NULL;
    deviceCacheClients = NULL;

    deviceCollisionCounts = NULL;
    hostCollisionCounts = NULL;
    
    deviceNextCollisionKeys = NULL;
    deviceCurrentCollisionKeys = NULL;
    deviceCorrectionCollisionKeys = NULL;
    deviceInsertCollisionKeys = NULL;

    hostCachePositions = NULL;
    hostCacheValues = NULL;
    hostCacheClients = NULL; 
    
    deviceOutputValues = NULL;
    deviceOutputIndex = NULL;
    deviceOutputClient = NULL;
    deviceOutputPosition = NULL;
    deviceOutputTotalValues = NULL;

    hostOutputValues = NULL;
    hostOutputIndex = NULL;
    hostOutputClient = NULL;
    hostOutputPosition = NULL;    
    hostOutputTotalValues = NULL;

    deviceTotalValues = NULL;
    hostTotalValues = NULL;

    deviceNewPositions = NULL;
    deviceNewValues = NULL;
    deviceNewClient = NULL;
    deviceNewNextDirections = NULL;
    deviceNewMovementIdx = NULL;
    deviceNewMovementPatternIdx = NULL;

    deviceOldPositions = NULL;
    deviceOldUpdateCounter = NULL;
    hostOldUpdateCounter = NULL;

    hostPositions = NULL;
    hostValues = NULL;
    hostClient = NULL;
    hostNextDirections = NULL;
    hostCollisions = NULL;
    hostMovementIdx = NULL;
    hostMovementPatternIdx = NULL;

    impacter = NULL;
    collision = NULL;
    inserter = NULL;
    linker = NULL;
}

void organisation::parallel::program::cleanup()
{
    if(dev != NULL) 
    {   
        sycl::queue q = ::parallel::queue(*dev).get();

        if(linker != NULL) delete linker;
        if(inserter != NULL) delete inserter;
        if(collision != NULL) delete collision;
        if(impacter != NULL) delete impacter;

        // ***
        if(hostMovementPatternIdx != NULL) sycl::free(hostMovementPatternIdx, q);
        if(hostMovementIdx != NULL) sycl::free(hostMovementIdx, q);
        if(hostCollisions != NULL) sycl::free(hostCollisions, q);
        if(hostNextDirections != NULL) sycl::free(hostNextDirections, q);
        if(hostClient != NULL) sycl::free(hostClient, q);
        if(hostValues != NULL) sycl::free(hostValues, q);
        if(hostPositions != NULL) sycl::free(hostPositions, q);
        // ***

        if(hostOldUpdateCounter != NULL) sycl::free(hostOldUpdateCounter, q);
        if(deviceOldUpdateCounter != NULL) sycl::free(deviceOldUpdateCounter, q);
        if(deviceOldPositions != NULL) sycl::free(deviceOldPositions, q);

        if(deviceNewMovementPatternIdx != NULL) sycl::free(deviceNewMovementPatternIdx, q);
        if(deviceNewMovementIdx != NULL) sycl::free(deviceNewMovementIdx, q);
        if(deviceNewNextDirections != NULL) sycl::free(deviceNewNextDirections, q);
        if(deviceNewClient != NULL) sycl::free(deviceNewClient, q);
        if(deviceNewValues != NULL) sycl::free(deviceNewValues, q);
        if(deviceNewPositions != NULL) sycl::free(deviceNewPositions, q);

// ***
        if(hostTotalValues != NULL) sycl::free(hostTotalValues, q);
        if(deviceTotalValues != NULL) sycl::free(deviceTotalValues, q);

// ***
        if(deviceOutputPosition != NULL) sycl::free(deviceOutputPosition, q);
        if(deviceOutputValues != NULL) sycl::free(deviceOutputValues, q);
        if(deviceOutputIndex != NULL) sycl::free(deviceOutputIndex, q);
        if(deviceOutputClient != NULL) sycl::free(deviceOutputClient, q);
        if(deviceOutputTotalValues != NULL) sycl::free(deviceOutputTotalValues, q);

        if(hostOutputPosition != NULL) sycl::free(hostOutputPosition, q);
        if(hostOutputValues != NULL) sycl::free(hostOutputValues, q);
        if(hostOutputIndex != NULL) sycl::free(hostOutputIndex, q);
        if(hostOutputClient != NULL) sycl::free(hostOutputClient, q);
        if(hostOutputTotalValues != NULL) sycl::free(hostOutputTotalValues, q);
        
// ***                
        if(hostCacheClients != NULL) sycl::free(hostCacheClients, q);
        if(hostCacheValues != NULL) sycl::free(hostCacheValues, q);
        if(hostCachePositions != NULL) sycl::free(hostCachePositions, q);

        if(deviceInsertCollisionKeys != NULL) sycl::free(deviceInsertCollisionKeys, q);
        if(deviceCorrectionCollisionKeys != NULL) sycl::free(deviceCorrectionCollisionKeys, q);
        if(deviceCurrentCollisionKeys != NULL) sycl::free(deviceCurrentCollisionKeys, q);        
        if(deviceNextCollisionKeys != NULL) sycl::free(deviceNextCollisionKeys, q);

        if(hostCollisionCounts != NULL) sycl::free(hostCollisionCounts, q);
        if(deviceCollisionCounts != NULL) sycl::free(deviceCollisionCounts, q);
        
        if(deviceCacheClients != NULL) sycl::free(deviceCacheClients, q);
        if(deviceCacheValues != NULL) sycl::free(deviceCacheValues, q);
        if(deviceCachePositions != NULL) sycl::free(deviceCachePositions, q);
        
        if(deviceClient != NULL) sycl::free(deviceClient, q);
        if(deviceLifetime != NULL) sycl::free(deviceLifetime, q);
        if(deviceMovementPatternIdx != NULL) sycl::free(deviceMovementPatternIdx, q);
        if(deviceMovementIdx != NULL) sycl::free(deviceMovementIdx, q);
        if(deviceValues != NULL) sycl::free(deviceValues, q);

        if(deviceMovementModifier != NULL) sycl::free(deviceMovementModifier, q);
        if(deviceNextDirections != NULL) sycl::free(deviceNextDirections, q);
        if(deviceNextHalfPositions != NULL) sycl::free(deviceNextHalfPositions, q);
        if(deviceNextPositions != NULL) sycl::free(deviceNextPositions, q);
        if(devicePositions != NULL) sycl::free(devicePositions, q);
    }    
}