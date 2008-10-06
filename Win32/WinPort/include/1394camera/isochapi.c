/**\file isochapi.c 
 * \brief Isochronous management functions
 * \ingroup capi
 */

/*
 *	Version 6.4
 *
 *  Copyright 8/2006
 *
 *  Christopher Baker
 *  Robotics Institute
 *  Carnegie Mellon University
 *  Pittsburgh, PA
 *
 *	Copyright 5/2000
 * 
 *	Iwan Ulrich
 *	Robotics Institute
 *	Carnegie Mellon University
 *	Pittsburgh, PA
 *
 *  This file is part of the CMU 1394 Digital Camera Driver
 *
 *  The CMU 1394 Digital Camera Driver is free software; you can redistribute 
 *  it and/or modify it under the terms of the GNU Lesser General Public License 
 *  as published by the Free Software Foundation; either version 2.1 of the License,
 *  or (at your option) any later version.
 *
 *  The CMU 1394 Digital Camera Driver is distributed in the hope that it will 
 *  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the CMU 1394 Digital Camera Driver; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "pch.h"

/**\brief Set up an isochronous stream.
 * \ingroup capi
 * \param szDeviceName The full pathname of the device 
 * \param pStreamParams The parameters to use
 * \return ERROR_SUCCESS on success, other codes on error
 * \see t1394IsochTearDownStream
 *
 * This is almost a direct wrapper over IOCTL_ISOCH_SETUP_STREAM, which
 * was introduced for version 6.4
 *
 * This will fail if an isoch stream has already been configured for the camera
 */
DWORD
CAMAPI
t1394IsochSetupStream(
	PSTR szDeviceName ,
	PISOCH_STREAM_PARAMS pStreamParams
	)
{
    HANDLE      hDevice;
    DWORD       dwRet, dwBytesRet;
	
    hDevice = OpenDevice(szDeviceName, FALSE);
	
    if (hDevice != INVALID_HANDLE_VALUE) {
		
        dwRet = DeviceIoControl( hDevice,
			IOCTL_ISOCH_SETUP_STREAM,
			pStreamParams,
			sizeof(ISOCH_STREAM_PARAMS),
			pStreamParams,
			sizeof(ISOCH_STREAM_PARAMS),
			&dwBytesRet,
			NULL
			);
        // Boolean: True is good, False needs GetLastError()
        dwRet = dwRet ? ERROR_SUCCESS : GetLastError();
        // free up resources
        CloseHandle(hDevice);
    } else {
		dwRet = GetLastError();
	}
	
    return(dwRet);
}

/**\brief Tear down an isochronous stream, if any.
 * \ingroup capi
 * \param szDeviceName The full pathname of the device 
 * \return ERROR_SUCCESS on success, other codes on error
 * \see t1394IsochSetupStream
 *
 * This is the counterpart to t1394IsochSetupStream.  It is not harmful to 
 * call it if no stream has been configured, and is in fact the only way to 
 * guarantee that the camera is idle before proceeding with the creation
 * of a new stream.
 */
DWORD
CAMAPI
t1394IsochTearDownStream(
	PSTR szDeviceName 
	)
{
	HANDLE			hDevice;
	DWORD 			dwRet, dwBytesRet;
	
	hDevice = OpenDevice(szDeviceName, FALSE);
	if (hDevice != INVALID_HANDLE_VALUE) {
		
		dwRet = DeviceIoControl(hDevice,
			IOCTL_ISOCH_TEAR_DOWN_STREAM,
			NULL,
			0,
			NULL,
			0,
			&dwBytesRet,
			NULL
			);
        // Boolean: True is good, False needs GetLastError()
        dwRet = dwRet ? ERROR_SUCCESS : GetLastError();
        // free up resources
        CloseHandle(hDevice);
	} else {
		dwRet = GetLastError();
	}
	
	return(dwRet);
}

/**\brief Attach a frame buffer to an isochronous stream.
 * \ingroup capi
 * \param hDevice The device handle to attach to.
 * \param pBuffer The frame buffer
 * \param ulBufferLength The length of teh frame buffer
 * \param pOverLapped The overlapped I/O structure to use for asynchronous notification
 * \return ERROR_SUCCESS or ERROR_IO_PENDING on success, other codes on error
 *
 * This will fail with ERROR_INSUFFICIENT_RESOURCES if no stream is in place per t1394IsochSetupStream()
 */
DWORD
CAMAPI
t1394IsochAttachBuffer(
    HANDLE hDevice,
    LPVOID pBuffer,
    ULONG  ulBufferLength,
    LPOVERLAPPED pOverLapped
    )
{
	DWORD 			dwRet, dwBytesRet;
	
    dwRet = DeviceIoControl(hDevice,
		IOCTL_ATTACH_BUFFER,
		NULL,
		0,
		pBuffer,
		ulBufferLength,
		&dwBytesRet,
		pOverLapped
		);
    // Boolean: True is good, False needs GetLastError()
    dwRet = dwRet ? ERROR_SUCCESS : GetLastError();
	
    return dwRet;
}

/**\brief Activate isochronous stream reception
 * \ingroup capi
 * \param szDeviceName The full pathname of the device 
 * \return ERROR_SUCCESS on success, other codes on error
 *
 * This is almost a direct wrapper over IOCTL_ISOCH_LISTEN, which
 * causes the 1394 bus subsystem to actually start shovelling data
 *
 * This will fail with ERROR_INSUFFICIENT_RESOURCES if no stream is in place per 
 * t1394IsochSetupStream() or if no buffers have been attached.
 */
DWORD
CAMAPI
t1394IsochListen(
    PSTR            szDeviceName
    )
{
    HANDLE      hDevice;
    DWORD       dwRet, dwBytesRet;
	
    hDevice = OpenDevice(szDeviceName, FALSE);
	
    if (hDevice != INVALID_HANDLE_VALUE) {
		
		dwRet = DeviceIoControl( hDevice,
			IOCTL_ISOCH_LISTEN,
			NULL,
			0,
			NULL,
			0,
			&dwBytesRet,
			NULL
			);
        // Boolean: True is good, False needs GetLastError()
        dwRet = dwRet ? ERROR_SUCCESS : GetLastError();
        // free up resources
        CloseHandle(hDevice);
    }
	
    return(dwRet);
} // IsochListen

/**\brief Dectivate isochronous stream reception
 * \ingroup capi
 * \param szDeviceName The full pathname of the device 
 * \return ERROR_SUCCESS on success, other codes on error
 *
 * This is almost a direct wrapper over IOCTL_ISOCH_STOP, which
 * stops the 1394 bus subsystem shovelling data
 *
 * This will fail with ERROR_INSUFFICIENT_RESOURCES if no stream is in place per 
 * t1394IsochSetupStream() or if no buffers have been attached.
 */
DWORD
CAMAPI
t1394IsochStop(
    PSTR            szDeviceName
    )
{
    HANDLE      hDevice;
    DWORD       dwRet, dwBytesRet;
	
    hDevice = OpenDevice(szDeviceName, FALSE);
	
    if (hDevice != INVALID_HANDLE_VALUE) {
		
		dwRet = DeviceIoControl( hDevice,
			IOCTL_ISOCH_STOP,
			NULL,
			0,
			NULL,
			0,
			&dwBytesRet,
			NULL
			);
        // Boolean: True is good, False needs GetLastError()
        dwRet = dwRet ? ERROR_SUCCESS : GetLastError();
        // free up resources
        CloseHandle(hDevice);
    }
	
    return(dwRet);
} // IsochStop

/**\brief Query the bus cycle time.
 * \ingroup capi
 * \param szDeviceName The full pathname of the device 
 * \param CycleTime where to put the data
 * \return ERROR_SUCCESS on success, other codes on error
 *
 * The uses of the Cycle Time are slim for us, so this may leave 
 * in a future version
 */
DWORD
CAMAPI
t1394IsochQueryCurrentCycleTime(
    PSTR            szDeviceName,
    PCYCLE_TIME     CycleTime
    )
{
    HANDLE      hDevice;
    DWORD       dwRet, dwBytesRet;
	
    hDevice = OpenDevice(szDeviceName, FALSE);
	
    if (hDevice != INVALID_HANDLE_VALUE) {
		
        dwRet = DeviceIoControl( hDevice,
			IOCTL_ISOCH_QUERY_CURRENT_CYCLE_TIME,
			NULL,
			0,
			CycleTime,
			sizeof(CYCLE_TIME),
			&dwBytesRet,
			NULL
			);
        // Boolean: True is good, False needs GetLastError()
        dwRet = dwRet ? ERROR_SUCCESS : GetLastError();
        // free up resources
        CloseHandle(hDevice);
    }
	
    return(dwRet);

} // IsochQueryCurrentCycleTime

/**\brief Query the available bus resources
 * \ingroup capi
 * \param szDeviceName The full pathname of the device 
 * \param isochQueryResources where to put the data
 * \return ERROR_SUCCESS on success, other codes on error
 *
 * Retrieve the available resources (channels, bandwidth, etc) for the bus
 * the camera is attached to.
 */
DWORD
CAMAPI
t1394IsochQueryResources(
    PSTR                        szDeviceName,
    PISOCH_QUERY_RESOURCES      isochQueryResources
    )
{
    HANDLE      hDevice;
    DWORD       dwRet, dwBytesRet;
	
    hDevice = OpenDevice(szDeviceName, FALSE);
	
    if (hDevice != INVALID_HANDLE_VALUE) {
		
        dwRet = DeviceIoControl( hDevice,
			IOCTL_ISOCH_QUERY_RESOURCES,
			isochQueryResources,
			sizeof(ISOCH_QUERY_RESOURCES),
			isochQueryResources,
			sizeof(ISOCH_QUERY_RESOURCES),
			&dwBytesRet,
			NULL
			);
        // Boolean: True is good, False needs GetLastError()
        dwRet = dwRet ? ERROR_SUCCESS : GetLastError();
        // free up resources
        CloseHandle(hDevice);
    }
	
    return(dwRet);
} // IsochQueryResources
