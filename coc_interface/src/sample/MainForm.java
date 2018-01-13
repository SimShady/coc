package sample;

import javax.swing.*;
import java.awt.*;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.*;
import java.util.Timer;
import java.util.TimerTask;

public class MainForm {
    private JList list_result;
    private JPanel main_panel;
    DefaultListModel list = new DefaultListModel();

    // Variables
    private static DatagramSocket socket = null;

    public MainForm(){
        list_result.setModel(list);
    }


    public static void broadcast(String broadcastMessage, InetAddress address) throws IOException {
        socket = new DatagramSocket();
        socket.setBroadcast(true);
        byte[] buffer = broadcastMessage.getBytes();
        DatagramPacket packet = new DatagramPacket(buffer, buffer.length, address, 44568);
        socket.send(packet);
        socket.close();
    }

    private static void init(MainForm mainForm){
        JFrame frame = new JFrame("App");
        frame.setContentPane(mainForm.main_panel);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.pack();
        // Center Frame Start
        Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
        int height = screenSize.height;
        int width = screenSize.width;
        frame.setSize(width/2, height/2);
        frame.setLocationRelativeTo(null);
        // Center Frame End

        frame.setVisible(true);
    }

    private static void startBroadcast(){
        TimerTask timerTask = new TimerTask() {
            @Override
            public void run() {
                try {
                    broadcast("Hello!", InetAddress.getByName("192.168.1.255"));
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        };

        Timer timer = new Timer(true);
        timer.schedule(timerTask, 0, 5000);
    }

    private static void TCPListener(MainForm mainForm){
        String clientSentence;
        String capitalizedSentence;
        ServerSocket tcpListener = null;
        try {
            tcpListener = new ServerSocket(44568);
        } catch (IOException e) {
            e.printStackTrace();
        }
        if(tcpListener != null){
            while (true) {
                try {
                    System.out.println("REIN?");
                    Socket connectionSocket = tcpListener.accept();
                    String senderIP = connectionSocket.getInetAddress().toString().replace("/", "");
                    BufferedReader inFromClient =
                            new BufferedReader(new InputStreamReader(connectionSocket.getInputStream()));
                    String streamLine = inFromClient.readLine();
                    if(!streamLine.split(" ")[1].equals(senderIP) && !mainForm.list
                            .contains(senderIP + " " + streamLine.split(" ")[0]))
                        mainForm.list.addElement(senderIP + " " + streamLine.split(" ")[0]);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        else System.out.println("Es ist ein Fehler aufgetreten! Der ServerSocket konnte nicht aufgebaut werden " +
                "da er mit null inizalisiert wurde");
    }

    public static void main(String[] args) {
        MainForm mainForm = new MainForm();
        init(mainForm);
        startBroadcast();
        TCPListener(mainForm);
    }
}
