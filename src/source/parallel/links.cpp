#include "parallel/links.hpp"

void organisation::parallel::links::reset(::parallel::device &dev, 
                                          ::parallel::queue *q, 
                                          parameters &settings)
{
    init = false; cleanup();

    this->dev = &dev;
    this->queue = q;
    this->settings = settings;
    this->length = settings.mappings.maximum() * settings.max_chain * settings.clients();

    sycl::queue &qt = ::parallel::queue(dev).get();

    deviceLinks = sycl::malloc_device<sycl::int4>(length, qt);
    if(deviceLinks == NULL) return;

    deviceLinkAge = sycl::malloc_device<int>(length, qt);
    if(deviceLinkAge == NULL) return;

    deviceLinkCount = sycl::malloc_device<int>(settings.mappings.maximum() * settings.clients(), qt);
    if(deviceLinkCount == NULL) return;

    clear();

    init = true;
}

void organisation::parallel::links::clear()
{
    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);

    std::vector<sycl::event> events;

    events.push_back(qt.memset(deviceLinks, -1, sizeof(sycl::int4) * length));
    events.push_back(qt.memset(deviceLinkAge, 0, sizeof(int) * length));
    events.push_back(qt.memset(deviceLinkCount, 0, sizeof(int) * settings.mappings.maximum() * settings.clients()));

    sycl::event::wait(events);
}

void organisation::parallel::links::copy(::organisation::genetic::links **source, int source_size)
{
    sycl::queue& qt = ::parallel::queue::get_queue(*dev, queue);

    sycl::int4 *hostLinks = sycl::malloc_host<sycl::int4>(settings.mappings.maximum() * settings.max_chain * settings.host_buffer, qt);    
    int *hostLinkCount = sycl::malloc_host<int>(settings.mappings.maximum() * settings.host_buffer, qt);
    
    if((hostLinks != NULL)&&(hostLinkCount != NULL))
    {
        memset(hostLinks, -1, sizeof(sycl::int4) * settings.mappings.maximum() * settings.max_chain * settings.host_buffer);
        memset(hostLinkCount, 0, sizeof(int) * settings.mappings.maximum() * settings.host_buffer);
        
        sycl::range num_items{(size_t)settings.clients()};

        int client_offset = settings.mappings.maximum() * settings.max_chain;
        int client_index = 0;
        int dest_index = 0;
        int index = 0;

        for(int source_index = 0; source_index < source_size; ++source_index)
        {
            organisation::genetic::links *links = source[source_index];
            int c_count = 0;        
            for(int i = 0; i < links->size(); ++i)
            {            
                point temp;
                if(links->get(temp,i))
                {
                    if((temp.x != -1)||(temp.y != -1)||(temp.z != -1))
                    {
                    hostLinks[c_count + (index * client_offset)] = { temp.x, temp.y, temp.z, 0 };
                    hostLinkCount[(c_count / settings.max_chain) + (index * settings.mappings.maximum())] += 1;
                    std::cout << "link count: " << hostLinkCount[(c_count % settings.max_chain) + (index * settings.mappings.maximum())] << "\n";
                    }
                    ++c_count;
                    if(c_count > client_offset) break;            
                }
            }

            ++index;
            ++client_index;

            if(index >= settings.host_buffer)
            {
                std::vector<sycl::event> events;

                events.push_back(qt.memcpy(&deviceLinks[dest_index * client_offset], hostLinks, sizeof(sycl::int4) * client_offset * index));        
                events.push_back(qt.memcpy(&deviceLinkCount[dest_index * settings.mappings.maximum()], hostLinkCount, sizeof(int) * settings.mappings.maximum() * index));        

                sycl::event::wait(events);
                
                memset(hostLinks, -1, sizeof(sycl::int4) * client_offset * settings.host_buffer);
                memset(hostLinkCount, 0, sizeof(int) * settings.mappings.maximum() * settings.host_buffer);
                            
                dest_index += settings.host_buffer;
                index = 0;            
            }
        }

        if(index > 0)
        {
            std::vector<sycl::event> events;

            events.push_back(qt.memcpy(&deviceLinks[dest_index * client_offset], hostLinks, sizeof(sycl::int4) * client_offset * index));        
            events.push_back(qt.memcpy(&deviceLinkCount[dest_index * settings.mappings.maximum()], hostLinkCount, sizeof(int) * settings.mappings.maximum() * index));        

            sycl::event::wait(events);
        }  
    }

    if(hostLinkCount != NULL) sycl::free(hostLinkCount, qt);
    if(hostLinks != NULL) sycl::free(hostLinks, qt);      
}

void organisation::parallel::links::outputarb(int *source, int length)
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

void organisation::parallel::links::outputarb(sycl::float4 *source, int length)
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

void organisation::parallel::links::makeNull()
{
    dev = NULL;

    deviceLinks = NULL;
    deviceLinkAge = NULL;
    deviceLinkCount = NULL;
}

void organisation::parallel::links::cleanup()
{
    if(dev != NULL) 
    {   
        sycl::queue q = ::parallel::queue(*dev).get();
    
        if(deviceLinkCount != NULL) sycl::free(deviceLinkCount, q);
        if(deviceLinkAge != NULL) sycl::free(deviceLinkAge, q);
        if(deviceLinks != NULL) sycl::free(deviceLinks, q);
    }
}
