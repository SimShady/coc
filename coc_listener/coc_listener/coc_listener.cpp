// coc_listener.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include <string>
#include <boost/asio.hpp>
#include <Windows.h>
#include <iostream>
#include <thread>

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


int main()
{
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
			string textNew{GetClipboardText()};
			while(textOld == textNew)
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

