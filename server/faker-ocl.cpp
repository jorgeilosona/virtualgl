// Copyright (C)2019 D. R. Commander
//
// This library is free software and may be redistributed and/or modified under
// the terms of the wxWindows Library License, Version 3.1 or (at your option)
// any later version.  The full license is in the LICENSE.txt file included
// with this distribution.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// wxWindows Library License for more details.

// Interposed OpenCL functions

#include "faker-sym.h"

using namespace vglserver;


extern "C" {

cl_context clCreateContext(const cl_context_properties *properties,
	cl_uint num_devices, const cl_device_id *devices,
	void pfn_notify(const char *errinfo, const void *private_info, size_t cb,
		void *user_data), void *user_data, cl_int *errcode_ret)
{
	cl_context_properties oclprops[257],
		*props = (cl_context_properties *)properties;
	int j = 0;

	if(properties)
	{
		memset(oclprops, 0, sizeof(cl_context_properties) * 257);
		for(int i = 0; properties[i] != 0 && i <= 254; i += 2)
		{
			oclprops[j++] = properties[i];
			if(properties[i] == CL_GLX_DISPLAY_KHR)
			{
				Display *dpy = (Display *)properties[i + 1];
				if(dpy && !IS_EXCLUDED(dpy))
				{
					oclprops[j++] = (cl_context_properties)(DPY3D);
					props = oclprops;
				}
			}
			else oclprops[j++] = properties[i + 1];
		}
	}

	return _clCreateContext(props, num_devices, devices, pfn_notify, user_data,
		errcode_ret);
}


}  // extern "C"
