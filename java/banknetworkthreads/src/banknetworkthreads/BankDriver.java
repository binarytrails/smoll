package banknetworkthreads;

public class BankDriver {

    /** 
     * main class
     * @param args the command line arguments
     */
    public static void main(String[] args) {
    	Network objNetwork = new Network("network");            /* Activate the network */
        objNetwork.start();
        Server objServer = new Server();                        /* Start the server */ 
        objServer.start();
        Client objClient1 = new Client("sending");              /* Start the sending client */
        objClient1.start();
        Client objClient2 = new Client("receiving");            /* Start the receiving client */
        objClient2.start();
        try {
			objNetwork.join();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
        System.out.println("==========Done==========");
    }
}
