
public class Test
{
	public static void main(String[] args)
	{
		OnBoardComputer system = new OnBoardComputer("HAL");
		Crew officer1 = new Crew("David", system);
		Crew officer2 = new Crew("Frank", system);
		
		/* 
		 *  System performs self-diagnostics.
		 */
		System.out.println("System performs self diagnostics:");
		System.out.println("================================");
		System.out.println("Crew member is " + officer1.toString() + ".");
		System.out.println("Crew member is " + officer2.toString() + ".");
		System.out.println(system.getMissionPurpose());
		System.out.println("\n");
		
		/*
		 *  David performs systems diagnostics.
		 */
		System.out.println("David performs system diagnostics:");
		System.out.println("================================");
		System.out.println(officer1.getSystemStatus());
		System.out.println(officer1.getSystemDate());
		System.out.println("\n");
		
		/*
		 *  Simulation of David's interaction.
		 */
		System.out.println("\nSimulation of David's interaction:");
		System.out.println("================================");
		System.out.println(officer1.whatIsPurposeOfMission());
		System.out.println(officer1.whatIsPurposeOfMission());
		System.out.println(officer1.whatIsPurposeOfMission());
		officer1.shutDownSystem();
		officer1.shutDownSystem();
		officer1.shutDownSystem();
		officer1.shutDownSystem();
		
		/*
		 *  Simulation of Frank's interaction.
		 */
		System.out.println("\nSimulation of Frank's interaction:");
		System.out.println("================================");
		System.out.println(officer2.whatIsPurposeOfMission());
		System.out.println(officer2.whatIsPurposeOfMission());
		System.out.println(officer2.whatIsPurposeOfMission());
		officer2.shutDownSystem();
		officer2.shutDownSystem();
		officer2.shutDownSystem();
	}
}
