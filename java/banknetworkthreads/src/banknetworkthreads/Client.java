package banknetworkthreads;

import java.util.Scanner;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.InputMismatchException;

public class Client extends Thread { 
    
    private static int numberOfTransactions;   		/* Number of transactions to process */
    private static int maxNbTransactions;      		/* Maximum number of transactions */
    private static Transactions [] transaction; 	/* Transactions to be processed */
    private static Network objNetwork;          	/* Client object to handle network operations */
    private String clientOperation;    				/* sending or receiving */
    
	/** Constructor method of Client class
 	 * 
     * @return 
     * @param
     */
     Client(String operation)
     { 
       if (operation.equals("sending"))
       { 
           System.out.println("\n Initializing client sending application ...");
           numberOfTransactions = 0;
           maxNbTransactions = 100;
           transaction = new Transactions[maxNbTransactions];  
           objNetwork = new Network("client");					// new instance??
           clientOperation = operation; 
           System.out.println("\n Initializing the transactions ... ");
           readTransactions();
           System.out.println("\n Connecting client to network ...");
           String cip = objNetwork.getClientIP();
           if (!(objNetwork.connect(cip)))
           {   System.out.println("\n Terminating client application, network unavailable");
               System.exit(0);
           }
       	}
       else
    	   if (operation.equals("receiving"))
           { 
    		   System.out.println("\n Initializing client receiving application ...");
    		   clientOperation = operation; 
           }
     }
           
    /** 
     * Accessor method of Client class
     * 
     * @return numberOfTransactions
     * @param
     */
     public synchronized int getNumberOfTransactions()
     {
         return numberOfTransactions;
     }
         
    /** 
     * Mutator method of Client class
     * 
     * @return 
     * @param nbOfTrans
     */
     public synchronized void setNumberOfTransactions(int nbOfTrans)
     { 
         numberOfTransactions = nbOfTrans;
     }
         
    /** 
     * Accessor method of Client class
     * 
     * @return clientOperation
     * @param
     */
     public String getClientOperation()
     {
         return clientOperation;
     }
         
    /** 
     * Mutator method of Client class
	 * 
	 * @return 
	 * @param operation
	 */
	 public void setClientOperation(String operation)
	 { 
	     clientOperation = operation;
	 }
         
    /** 
     * Reading of the transactions from an input file
     * 
     * @return 
     * @param
     */
     public void readTransactions()
     {
        Scanner inputStream = null;     /* Transactions input file stream */
        int i = 0;                      /* Index of transactions array */
        
        try
        {
        	inputStream = new Scanner(new FileInputStream("transaction.txt"));
        }
        catch(FileNotFoundException e)
        {
            System.out.println("File transaction.txt was not found");
            System.out.println("or could not be opened.");
            System.exit(0);
        }
        while (inputStream.hasNextLine( ))
        {
        	synchronized(this) {
            try
            {   transaction[i] = new Transactions();
                transaction[i].setAccountNumber(inputStream.next());            /* Read account number */
                transaction[i].setOperationType(inputStream.next());            /* Read transaction type */
                transaction[i].setTransactionAmount(inputStream.nextDouble());  /* Read transaction amount */
                transaction[i].setTransactionStatus("pending");                 /* Set current transaction status */
                i++;
            }
             catch(InputMismatchException e)
            {
                System.out.println("Line " + i + "file transactions.txt invalid input");
                System.exit(0);
            }
        	}
            
        }
        setNumberOfTransactions(i);		/* Record the number of transactions processed */
        
        //System.out.println("\n DEBUG : Client.readTransactions() - " + getNumberOfTransactions() + " transactions processed");
        
        inputStream.close( );

     }
     
    /** 
     * Sending the transactions to the server 
     * 
     * @return 
     * @param
     */
     public void sendTransactions()
     {
         int i = 0;     /* index of transaction array */
         
         while (i < getNumberOfTransactions())
         {  
            /* Alternatively, busy-wait until the network input buffer is available */
            while(objNetwork.getInBufferStatus().equals("full")){
                Thread.yield(); // not doing anything important, let others run
            }
            synchronized(this) {                             	
            transaction[i].setTransactionStatus("sent");   /* Set current transaction status */
            }
           
            //System.out.println("\n DEBUG : Client.sendTransactions() - sending transaction on account " + transaction[i].getAccountNumber());
            
            objNetwork.send(transaction[i]);                            /* Transmit current transaction */
            i++;
         }
         
    }
         
 	/** 
  	 * Receiving the completed transactions from the server
     * 
     * @return 
     * @param transact
     */
     public void receiveTransactions(Transactions transact)
     {
         int i = 0;     /* Index of transaction array */
         
         while (i < getNumberOfTransactions())
         {     
            /* Alternatively, busy-wait until the network output buffer is available */
        	while(objNetwork.getOutBufferStatus().equals("empty")){
                Thread.yield(); // not doing anything important, let others run
            }
                                                                        	
            objNetwork.receive(transact);                               	/* Receive updated transaction from the network buffer */
            
            //System.out.println("\n DEBUG : Client.receiveTransactions() - receiving updated transaction on account " + transact.getAccountNumber());
            
            System.out.println(transact);                               	/* Display updated transaction */    
            i++;

            //System.out.println(i + " =? " + getNumberOfTransactions());
         } 
    }
     
    /** 
     * Create a String representation based on the Client Object
     * 
     * @return String representation
     * @param 
     */
     public synchronized String toString() 
     {
    	 return ("\n client IP " + objNetwork.getClientIP() + " Connection status" + objNetwork.getClientConnectionStatus() + "Number of transactions " + getNumberOfTransactions());
     }
    
    /** Code for the run method
     * 
     * @return 
     * @param
     */
    public void run()
    {   
    	Transactions transact = new Transactions();
    	long runningTime = 0,
    		 sendClientStartTime, sendClientEndTime,
    		 receiveClientStartTime, receiveClientEndTime;
    
    	/* Implement the code for the run method */
        // wait for the network to be ready
        while(!objNetwork.getNetworkStatus().equals("active")){
            Thread.yield(); // if not let other threads run
    	}
        objNetwork.connect(objNetwork.getClientIP());
    	if (clientOperation.equals("sending")) {
	    	sendClientStartTime = System.currentTimeMillis();
	    	sendTransactions();
	    	sendClientEndTime = System.currentTimeMillis();
	    	runningTime = sendClientEndTime - sendClientStartTime;
	    	//objNetwork.disconnect(objNetwork.getClientIP());
    	}
    	else if (clientOperation.equals("receiving")) {
	    	receiveClientStartTime = System.currentTimeMillis();
	    	receiveTransactions(transact);
	    	receiveClientEndTime = System.currentTimeMillis();
	    	runningTime = receiveClientEndTime - receiveClientStartTime;
	    	// since different sequential client operations in two threads, we only disconnect after we got it all
	    	objNetwork.disconnect(objNetwork.getClientIP());
    	}
    	System.out.println("\n Terminating client " + clientOperation + " thread - " + " Running time " + runningTime + " milliseconds");
    }
}
