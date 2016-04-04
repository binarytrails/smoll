
public class Crew extends Personnel {
	
	public Crew(String name, OnBoardComputer system) {
		super(name, system);
	}
	
	public String getSystemStatus() {
		return system.getStatus();
	}
	
	public String getSystemDate() {
		return system.getDate();
	}
	
	public String whatIsPurposeOfMission() {
		return system.getMissionPurpose();
	}
	
	public void shutDownSystem() {
		system.shutDown();
	}

}
