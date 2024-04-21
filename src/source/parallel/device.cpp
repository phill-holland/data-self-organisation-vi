#include "parallel/device.hpp"
#include "parallel/queue.hpp"

parallel::device::device(int index)
{
    dev = select(index);
}

std::vector<int> parallel::device::get(int *source, int length)
{
	int *temp = new int[length];
	if (temp == NULL) return { };

    sycl::queue q = ::parallel::queue(*this).get();

    q.memcpy(temp, source, sizeof(int) * length).wait();

    std::vector<int> result;

	for (int i = 0; i < length; ++i)
	{
        result.push_back(temp[i]);
	}

	delete[] temp;

    return result;
}

std::vector<sycl::float4> parallel::device::get(sycl::float4 *source, int length)
{
    sycl::float4 *temp = new sycl::float4[length];
    if (temp == NULL) return { };

    sycl::queue q = ::parallel::queue(*this).get();

    q.memcpy(temp, source, sizeof(sycl::float4) * length).wait();

    std::vector<sycl::float4> result;
    
	for (int i = 0; i < length; ++i)
	{
        int ix = (int)(temp[i].x() * 100.0f);
        int iy = (int)(temp[i].y() * 100.0f);
        int iz = (int)(temp[i].z() * 100.0f);

        result.push_back(temp[i]);					
	}
	
	delete[] temp;

    return result;
}

std::vector<sycl::int4> parallel::device::get(sycl::int4 *source, int length)
{
    sycl::int4 *temp = new sycl::int4[length];
    if (temp == NULL) return { };

    sycl::queue q = ::parallel::queue(*this).get();

    q.memcpy(temp, source, sizeof(sycl::int4) * length).wait();

    std::vector<sycl::int4> result;
    
	for (int i = 0; i < length; ++i)
	{
        int ix = (int)(temp[i].x() * 100.0f);
        int iy = (int)(temp[i].y() * 100.0f);
        int iz = (int)(temp[i].z() * 100.0f);

        result.push_back(temp[i]);			
	}
	
	delete[] temp;

    return result;
}

std::vector<std::string> parallel::device::enumerate()
{
	std::vector<std::string> result;

	for (auto device : sycl::device::get_devices(sycl::info::device_type::gpu))
	{
		result.push_back(device.get_info<sycl::info::device::name>());
  	}

	return result;
}

std::string parallel::device::capabilities()
{
    std::string result;
	int count = 0;

	for (auto device : sycl::device::get_devices(sycl::info::device_type::gpu))//all))
	{
		std::string name = device.get_info<sycl::info::device::name>();
		pi_uint32 grids = device.get_info<sycl::info::device::max_work_item_dimensions>();

        result += std::to_string(count);
        result += std::string(") Name ");
        result += name;
        result += std::string("\r\n");

        result += std::string("Max Grids ");
        result += std::to_string(grids);
        result += std::string("\r\n");

		++count;
	}

    return result;
}

sycl::device parallel::device::select(int index)
{
    std::vector<sycl::device> devices = sycl::device::get_devices(sycl::info::device_type::gpu);
    if((index >= 0)&&(index < devices.size()))
        return devices.at(index);

    return devices.at(0);
}
