// Include the basic windows header file and the Direct3D header files
#include <Windows.h>
#include <d3d11.h>

// Include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")

// Define screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

// Global declarations
IDXGISwapChain *swapchain;			// the pointer to the swap chain interface
ID3D11Device *device;				// the pointer to our Direct3D device interface
ID3D11DeviceContext *deviceContext;	// the pointer to our Direct3D device context
ID3D11RenderTargetView *backbuffer; // the pointer to an object that holds all the information about the render target

// Function prototypes
void InitD3D( HWND hWnd );	// sets up and initializes Direct3D
void RenderFrame( void );	// Renders a single frame
void CleanD3D( void );		// closes Direct3D and releases memory

// The WindowProc function prototype
LRESULT CALLBACK WindowProc( HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam );

// The entry point for any Windows program
int WINAPI WinMain( HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow )
{
	// The handle for the window, filled by a function
	HWND hWnd;
	// This struct holds information for the window class
	WNDCLASSEX wc;

	// Clear out the window class for use
	ZeroMemory( &wc, sizeof( WNDCLASSEX ) );

	// Fill in the struct with the needed information
	wc.cbSize = sizeof( WNDCLASSEX );
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	// wc.hbrBackground = (HBRUSH)COLOR_WINDOW;		// no longer needed
	wc.lpszClassName = L"WindowClass1";

	// Register the window class
	RegisterClassEx( &wc );

	// Calculate the size of the client area
	RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }; // Set the size but not the position
	AdjustWindowRect( &wr, WS_OVERLAPPEDWINDOW, FALSE ); // Adjust the size

	// Create the Window and use the result as the handle
	hWnd = CreateWindowEx( NULL,
		L"WindowClass1",				// Name of the window class
		L"Our First Direct3D Program",	// Title of the window
		WS_OVERLAPPEDWINDOW,			// Window style
		300,							// x-position of the window
		300,							// y-position of the window
		wr.right - wr.left,
		wr.bottom - wr.top,					// height of the window
		NULL,							// We have no parent window, NULL
		NULL,							// We aren't using menus, NULL
		hInstance,						// Application handle	
		NULL );							// Used with multiple windows, NULL

	// Display the window on the screen
	ShowWindow( hWnd, nCmdShow );

	// Set up and initialize Direct3D
	InitD3D( hWnd );

	// Enter the main loop:

	// This struct holds Windows event messages
	MSG msg = { 0 };

	// Enter the infinite message loop
	while (TRUE)
	{
		// Check to see if any messages are waiting int the queue
		if (PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ))
		{
			// Translate keystroke messages into the right format
			TranslateMessage( &msg );

			// Send the message to the WindowProc function
			DispatchMessage( &msg );

			// Check to see if it's time to quit
			if (msg.message == WM_QUIT)
				break;
		}
		
		// Run the game code here
		RenderFrame();
	}

	// Clean up DirectX and COM
	CleanD3D();

	// Return this part of the WM_QUIT message to Windows
	return msg.wParam;
}

// This is the main message handler for the program
LRESULT CALLBACK WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	// Sort through and find what code to run for the message given
	switch (message)
	{
		// This message is read when the window is closed
		case WM_DESTROY:
		{
			// Close the application entirely
			PostQuitMessage( 0 );
			return 0;
		} break;
	}

	// Handle any messages the switch statement didn't
	return DefWindowProc( hWnd, message, wParam, lParam );
}

// This function initializes and prepares Direct3D for use
void InitD3D( HWND hWnd )
{
	// Direct3D initialization
	// Create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// Clear out the struct for use
	ZeroMemory( &scd, sizeof( DXGI_SWAP_CHAIN_DESC ) );

	// Fill the swap chain description struct
	scd.BufferCount = 1;									// one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		// use 32-bit color
	scd.BufferDesc.Width = SCREEN_WIDTH;					// set the back buffer width
	scd.BufferDesc.Height = SCREEN_HEIGHT;					// set the back buffer height
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// how swap chain is to be used
	scd.OutputWindow = hWnd;								// the window to be used
	scd.SampleDesc.Count = 4;								// how many multisamples
	scd.Windowed = TRUE;									// windowed/full-screen mode
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;		// allow full-screen switching

	// Create a device, device context and swap chain 
	// using the information in the scd struct
	D3D11CreateDeviceAndSwapChain( NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&swapchain,
		&device,
		NULL,
		&deviceContext );

	// Set the render target
	// Get the address of the back buffer
	ID3D11Texture2D *pBackBuffer;
	swapchain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer );

	// Use the back buffer address to create the render target
	device->CreateRenderTargetView( pBackBuffer, NULL, &backbuffer );
	pBackBuffer->Release();

	// Set the render target as the back buffer
	deviceContext->OMSetRenderTargets( 1, &backbuffer, NULL );

	// Set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory( &viewport, sizeof( D3D11_VIEWPORT ) );

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;

	deviceContext->RSSetViewports( 1, &viewport );
}


// This is the function used to render a single frame
void RenderFrame( void )
{
	// Clear backbuffer to blue
	float color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };

	// Clear the back buffer to blue
	deviceContext->ClearRenderTargetView( backbuffer, color);

	// Do 3D rendering on the back buffer here

	// Switch the back buffer and the front buffer
	swapchain->Present( 0, 0 );
}

// This is the function that cleans up Direct3D and COM
void CleanD3D()
{
	swapchain->SetFullscreenState( FALSE, NULL );	// switch to windowed mode

	// Close and release all existing COM objects
	swapchain->Release();
	backbuffer->Release();
	device->Release();
	deviceContext->Release();
}