#include "parallel/collisions.hpp"

void organisation::parallel::collisions::reset(::parallel::device &dev, 
                                               ::parallel::queue *q, 
                                               parameters &settings)
{
    init = false; cleanup();

    this->dev = &dev;
    this->queue = q;
    this->settings = settings;
    this->length = settings.max_collisions * settings.mappings.maximum() * settings.clients();

    sycl::queue &qt = ::parallel::queue(dev).get();

    deviceCollisions = sycl::malloc_device<sycl::float4>(length, qt);
    if(deviceCollisions == NULL) return;

    hostCollisions = sycl::malloc_host<sycl::float4>(settings.max_collisions * settings.mappings.maximum() * settings.host_buffer, qt);
    if(hostCollisions == NULL) return;

    clear();

    init = true;
}

void organisation::parallel::collisions::clear()
{
    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);

    std::vector<sycl::event> events;

    events.push_back(qt.memset(deviceCollisions, 0, sizeof(sycl::float4) * length));

    sycl::event::wait(events);
}

void organisation::parallel::collisions::copy(::organisation::schema **source, int source_size)
{
    memset(hostCollisions, 0, sizeof(sycl::float4) * settings.max_collisions * settings.mappings.maximum() * settings.host_buffer);
    
    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);
    sycl::range num_items{(size_t)settings.clients()};

    int client_offset = settings.max_collisions * settings.mappings.maximum();
    int client_index = 0;
    int dest_index = 0;
    int index = 0;

    for(int source_index = 0; source_index < source_size; ++source_index)
    {
        organisation::program *prog = &source[source_index]->prog;

        int c_count = 0;        
        for(int i = 0; i < prog->collisions.size(); ++i)
        {
            int direction;
            if(prog->collisions.get(direction,i))
            {
                vector temp;                
                temp.decode(direction);
                hostCollisions[c_count + (index * client_offset)] = { (float)temp.x, (float)temp.y, (float)temp.z, 0.0f };
                ++c_count;
                if(c_count > client_offset) break;            
            }
        }

        ++index;
        ++client_index;

        if(index >= settings.host_buffer)
        {
            std::vector<sycl::event> events;

            events.push_back(qt.memcpy(&deviceCollisions[dest_index * client_offset], hostCollisions, sizeof(sycl::float4) * client_offset * index));        

            sycl::event::wait(events);
            
            memset(hostCollisions, 0, sizeof(sycl::float4) * client_offset * settings.host_buffer);
                        
            dest_index += settings.host_buffer;
            index = 0;            
        }
    }

    if(index > 0)
    {
        std::vector<sycl::event> events;

        events.push_back(qt.memcpy(&deviceCollisions[dest_index * client_offset], hostCollisions, sizeof(sycl::float4) * client_offset * index));        

        sycl::event::wait(events);
    }        
}

void organisation::parallel::collisions::into(::organisation::schema **destination, int destination_size)
{
    int client_offset = settings.max_collisions * settings.mappings.maximum();

    int src_client_index = 0;
    int dest_client_index = 0;
    
    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);
    sycl::range num_items{(size_t)settings.clients()};

    do
    {
        qt.memcpy(hostCollisions, &deviceCollisions[src_client_index * client_offset], sizeof(sycl::float4) * client_offset * settings.host_buffer).wait();

        for(int i = 0; i < settings.host_buffer; ++i)
        {
            organisation::program *prog = &destination[dest_client_index]->prog;

            for(int j = 0; j < client_offset; ++j)
            {
                sycl::float4 direction = hostCollisions[(i * client_offset) + j];
                vector temp((int)direction.x(), (int)direction.y(), (int)direction.z());
                prog->collisions.set(temp.encode(), j);
            }

            ++dest_client_index;
            if(dest_client_index >= destination_size) return;
        }        

        src_client_index += settings.host_buffer;
    } while((src_client_index * client_offset) < length);
}

void organisation::parallel::collisions::outputarb(int *source, int length)
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

void organisation::parallel::collisions::outputarb(sycl::float4 *source, int length)
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

void organisation::parallel::collisions::makeNull()
{
    dev = NULL;

    deviceCollisions = NULL;
    hostCollisions = NULL;
}

void organisation::parallel::collisions::cleanup()
{
    if(dev != NULL) 
    {   
        sycl::queue q = ::parallel::queue(*dev).get();

        if(hostCollisions != NULL) sycl::free(hostCollisions, q);
        if(deviceCollisions != NULL) sycl::free(deviceCollisions, q);          
    }
}
