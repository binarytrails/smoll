import java.io.BufferedWriter;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.Writer;

public aspect Logger
{
	private String systemLogsFileName = "system-logs.txt";
	
	private Logger()
	{
		this.writeToSystemLogFile("", false); // clears the file.
	}
	
	private String formatSystemLog(String caller, String callee, String method)
	{
		String mstime = String.valueOf(System.currentTimeMillis() % 1000);
		return mstime + " : " + caller + " : " + callee + " : " + method;
	}
	
	private void writeToSystemLogFile(String log, boolean append)
	{
		try (Writer writer = new BufferedWriter(new OutputStreamWriter(
				new FileOutputStream(systemLogsFileName, append), "utf-8")))
		{
		   writer.write(log + "\n");
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
	}
	
	pointcut crewGettingOnBoardComputerInfo(Crew officer, OnBoardComputer computer):
		call(* OnBoardComputer.*(..)) && this(officer) && target(computer);
	
	before(Crew officer, OnBoardComputer computer): crewGettingOnBoardComputerInfo(officer, computer)
	{
		String log = formatSystemLog(officer.getClass().getName(), officer.system.name,
				thisJoinPoint.getSignature().getName());

		this.writeToSystemLogFile(log, true);
	}
	
	pointcut lifeSupportGettingCrewLifeStatus(LifeSupport lifeSupport, Crew officer): 
		call(* Crew.getLifeStatus()) && this(lifeSupport) && target(officer);
	
	before(LifeSupport lifeSupport, Crew officer): lifeSupportGettingCrewLifeStatus(lifeSupport, officer)
	{
		String log = formatSystemLog(lifeSupport.getClass().getName(), officer.name,
				thisJoinPoint.getSignature().getName());

		this.writeToSystemLogFile(log, true);
	}
	
	pointcut authorizationKillingCrew(Authorization auth, Crew officer):
		call(void Authorization.kill(Crew)) && target(auth) && args(officer);
	
	before(Authorization auth, Crew officer): authorizationKillingCrew(auth, officer)
	{
		String log = formatSystemLog(auth.getClass().getName(), officer.name,
				thisJoinPoint.getSignature().getName());
		
		this.writeToSystemLogFile(log, true);
	}
}
