
import java.text.SimpleDateFormat;
import java.util.Date;


public class Computer {

	protected boolean active;
	protected String name;
	
	protected String description = "Keep the discovery of the Monolith TMA-1 a secret.";
	
	public Computer(String name) {
		this.name = name;
		this.active = true;
	}
	
	public String getStatus() {
		if (active == true)
			return name + " is active.";
		else
			return null;
	}
	
	public String getDate() {
		Date d = new Date();
	    SimpleDateFormat form = new SimpleDateFormat("dd-mm-yyyy hh:mm:ss");
	    return form.format(d);
	}
		
	public void shutDown() {
		this.active = false;
	}
	
	public String toString() {
		return name;
	}

}

