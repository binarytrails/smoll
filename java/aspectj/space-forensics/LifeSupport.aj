
public privileged aspect LifeSupport
{
	declare parents: Crew implements LifeStatus;
	
	private boolean Crew.isAlive = true;
	
	public boolean Crew.getLifeStatus()
	{
		return this.isAlive;
	}
	
	pointcut crewMessagesToOnBoardComputer(Crew officer): call(* OnBoardComputer.*(..)) && this(officer);
	
	void around(Crew officer): crewMessagesToOnBoardComputer(officer)
	{
		if(officer.getLifeStatus())
		{
			proceed(officer);
		}
	}
	
	String around(Crew officer): crewMessagesToOnBoardComputer(officer)
	{
		if(officer.getLifeStatus())
		{
			return proceed(officer);
		}
		return "";
	}
}

