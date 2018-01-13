// coc_listener.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include <string>
#include <boost/asio.hpp>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <boost/array.hpp>

using namespace std;
using namespace boost::asio::ip;

string GetClipboardText()
{
	while(true){
		if (!OpenClipboard(nullptr))
		{
			cout << "FEHLER" << endl;
			CloseClipboard();
			this_thread::sleep_for(1s);
			continue;
		}

		HANDLE hData = GetClipboardData(CF_TEXT);
		if (hData == nullptr)
		{
			cout << "FEHLER" << endl;
			CloseClipboard();
			this_thread::sleep_for(1s);
			continue;
		}
		char * pszText = static_cast<char*>(GlobalLock(hData));
		if (pszText == nullptr)
		{
			cout << "FEHLER" << endl;
			CloseClipboard();
			this_thread::sleep_for(1s);
			continue;
		}
		string text(pszText);

		GlobalUnlock(hData);

		CloseClipboard();

		return text;
	}
}


void copy_service() {
	int i{ 0 };
	string textOld{ GetClipboardText() };
	short unsigned int port = 44567;

	boost::asio::io_service ctx;
	tcp::endpoint endpoint{ tcp::v4(), port };
	tcp::acceptor acceptor{ ctx, endpoint };

	acceptor.listen();

	tcp::iostream stream;

	while (true) {
		try {
			i++;
			cout << "Warte auf eine Anfrage - " << i << endl;
			acceptor.accept(*stream.rdbuf());
			cout << "Anfrage kam" << endl;
		}
		catch (exception e) {
			break;
		}

		try {
			string textNew{ GetClipboardText() };
			while (textOld == textNew)
			{
				textNew = GetClipboardText();
				this_thread::sleep_for(1s);
			}
			textOld = textNew;
			stream << textNew;
		}
		catch (exception e) {
			cout << "Fehler!" << endl;
		}
		stream.close();
	}
}

void alive_service() {
	short unsigned int port = 44568;
	char hostname[128];
	gethostname(hostname, sizeof(hostname));

	try {

		boost::asio::io_service io_service;
		udp::socket socket{ io_service, udp::endpoint(udp::v4(), port) };

		while (true) {
			boost::array<char, 6> recv_buf;
			udp::endpoint remote_endpoint;
			boost::system::error_code error;
			socket.receive_from(boost::asio::buffer(recv_buf),
				remote_endpoint, 0, error);

			if (error && error != boost::asio::error::message_size)
				throw boost::system::system_error(error);

			for (int i = 0; i < recv_buf.size(); i++) {
				cout << recv_buf[i];
			}
			
			auto address = remote_endpoint.address();

			tcp::iostream stream{ address.to_string(), to_string(port) };

			try {
				stream << hostname << " " << address.to_string();
				stream.close();
			}
			catch (exception e) {
				cout << e.what() << endl;
			}

		}
	}
	catch (exception e) {
		cout << e.what() << endl;
		return;
	}
}


int main()
{
	thread copy_service_thread{ copy_service };
	thread alive_service_thread{ alive_service };

	copy_service_thread.join();
	alive_service_thread.join();
}

