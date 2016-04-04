public class OnBoardComputer extends Computer {
	
	private String description = "Relay information accurately without distortion or concealment.";
	
	public OnBoardComputer(String name) {
		super(name);
	}
	
	public String getMissionPurpose() {
		return ">>" + super.description + "\n" + this.description + "<<";
	}
	
	public void shutDown() {
		super.shutDown();
		this.relayShutDownMessage();
	}
	
	private void relayShutDownMessage() {
		System.out.println("Daisy Bell...");
	}
	
	
}
