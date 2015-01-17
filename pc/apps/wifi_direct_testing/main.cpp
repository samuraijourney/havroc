#include <iostream>
#include <communications/wifi_direct.h>

int main() {
	WifiDirect* obj = new WifiDirect();
	std::cout << obj->addNumber(4,7) << std::endl;
	delete obj;
	return 0;
}
