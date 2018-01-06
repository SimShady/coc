package com.company;

import java.awt.*;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.StringSelection;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.Socket;

public class Main {

    public static void main(String[] args) {
        String hostName = "192.168.0.66";
        int portNumber = 44567;

        try {
            while(true) {
                Socket echoSocket = new Socket(hostName, portNumber);
                BufferedReader in =
                        new BufferedReader(
                                new InputStreamReader(echoSocket.getInputStream()));

                String line;

                String result = "";

                while((line = in.readLine()) != null){
                    result += line + "\n";
                }
                System.out.println(result);
                StringSelection selection = new StringSelection(result);
                Clipboard clipboard = Toolkit.getDefaultToolkit().getSystemClipboard();
                clipboard.setContents(selection, selection);
            }
        } catch(Exception e){
            System.out.println("Ups ich bin eins spast, hat nix funktioniert! Hopala da is wos schief gelaufen!");
        }

    }
}
