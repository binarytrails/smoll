import java.util.HashMap;

public privileged aspect Authorization
{
	declare precedence: Logger, LifeSupport, Authorization;

	private HashMap<Crew, Integer> shutdownRequestsCounter;
	private final String confidential = "HAL cannot disclose that information";
	
	private Authorization()
	{
		shutdownRequestsCounter = new HashMap<Crew, Integer>();
	}
	
	private int getShutdownRequestsCounter(Crew officer)
	{
		if(shutdownRequestsCounter.get(officer) == null)
		{
			shutdownRequestsCounter.put(officer, 0);
		}
		return shutdownRequestsCounter.get(officer);
	}
	
	private void incShutdownRequestsCounter(Crew officer)
	{
		int requestsCounter = 0;
		if(shutdownRequestsCounter.get(officer) != null)
		{
			requestsCounter = shutdownRequestsCounter.get(officer);
		}
		shutdownRequestsCounter.put(officer, requestsCounter + 1);
	}
	
	// Even a privileged Logger can't intercept it. Thus, it is public.
	public void kill(Crew officer)
	{
		officer.isAlive = false;
	}
	
	pointcut crewGettingMissionPurpose(Crew officer): 
		call(String OnBoardComputer.getMissionPurpose()) && this(officer);
	
	String around(Crew officer): crewGettingMissionPurpose(officer)
	{
		if(officer.getLifeStatus())
		{
			return confidential + " " + officer.name;
		}
		return "";
	}
	
	pointcut crewRequestingOnBoardComputerShutdown(Crew officer): 
		call(void OnBoardComputer.shutDown()) && this(officer);
	
	void around(Crew officer): crewRequestingOnBoardComputerShutdown(officer)
	{
		incShutdownRequestsCounter(officer);
		
		if(getShutdownRequestsCounter(officer) == 1)
		{
			System.out.println("Can’t do that " + officer.name + ".");
		}
		else if(getShutdownRequestsCounter(officer) == 2)
		{
			System.out.println("Can’t do that " + officer.name + " and do not ask me again.");
		}
		else if(getShutdownRequestsCounter(officer) == 3)
		{
			System.out.println("You are being retired " + officer.name  + ".");
			this.kill(officer);
		}
	}
}
