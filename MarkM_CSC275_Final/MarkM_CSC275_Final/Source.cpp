/* Mark Montenieri
   Spring 2021
   CSC275

   Final project */

#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <sstream>
#include <fstream>
#include <chrono>
#include <condition_variable>
#include <random>
#include <limits> 

using namespace std;

mutex Locker; // The mutex class is a synchronization primitive that can be used to protect shared data from being simultaneously accessed by multiple threads
condition_variable cv;
string mainThreadID;// Main thread ID
string fileName = "VehicleData.txt";// File name
int chevySuperchargerHP = 200;
double chevyWiderTireTraction = 0.5;
int dodgeSuperchargerHP = 200;
double dodgeWiderTireTraction = 0.5;

struct AddonsInstalled
{
	bool chevyTires = false;
	bool chevySupercharger = false;
	bool dodgeTires = false;
	bool dodgeSupercharger = false;
};
 

int mainMenu()
{
	int choice;
	cout << "1 = Rules" << endl;
	cout << "2 = RACE!" << endl;
	cout << "3 = Chevy Speed Shop (Will also load profile if one is present)" << endl;
	cout << "4 = Dodge Speed Shop (Will also load profile if one is present)" << endl;
	cout << "5 = Load Chevy profile" << endl;
	cout << "6 = Load Dodge profile" << endl;	
	cout << "7 = Show best runs" << endl;
	cout << "8 = Exit" << endl;

	 	 
	cin >> choice;// Attempt to get user input
	 
	/* Exception handling */
	try
	{
		while (!cin.good())// Do while value is not good
		{
			cin.clear();
			cin.ignore(std::numeric_limits<streamsize>::max(), '\n');// Ensure value is numeric
			cout << "You must type an integer." << endl << endl;
			choice = mainMenu();// call function again
		}
	}
	catch (string ex)
	{
		cout << "Input error: " << ex << endl;
	}
		 
	return choice;
}; // End of menu

int speedShopMenu(bool tires, bool supercharger)
{
	int choice;
	 if (tires)// Wider tires installed?
	 {
		 cout << "Wider tires INSTALLED (More traction)" << endl;
	 }
	 else
	 {
		 cout << "1 = Add Wider tires? (More traction)" << endl;
	 }
	
	if (supercharger)// Supercharger installed?
	{
		cout << "Supercharger INSTALLED (more horsepower)" << endl;
	}
	else
	{
		cout << "2 = Add Supercharger? (more horsepower)" << endl;
	}
		 
	cout << "3 = Exit Speed Shop" << endl;
	cin >> choice;
	return choice;
}; // End of menu

class GameStructure
{
public:
	void Rules(string chevyDesc, string dodgeDesc)
	{
		cout << "Cars can be raced in their stock form by choose race from the menu" << endl;// Race as-is
		cout << "or you can customize each car in the speed shops." << endl << endl;// Speed shop
		cout << "To save what you have installed on a specific vehicle, choose the save option for" << endl;// Save
		cout << "that vehicle and the current configuration will be saved to file. This must be done for each" << endl;
		cout << "vehicle that has been customized!" << endl << endl;		
		cout << "If you had previously saved a customized vehicle, you can also load it from file before you race." << endl << endl;// Load
		cout << "Currently installed add-ons for each vehicle can be viewed via the menu" << endl;// View
		cout << "You can also view the list of fastest (best) runs by choosing that menu option." << endl << endl;

		cout << "Each vehicle has a strength and a weakness, so choose your performance add-ons wisely." << endl;
		cout << chevyDesc << endl;
		cout << dodgeDesc << endl;
	}

	virtual int RandomRoll()// Can be overridden
	{	 
		/* Roll random number */
		random_device rd; // obtain a random number
		mt19937 gen(rd()); // seed
		uniform_int_distribution<> distr(10, 200); // define the range
		 
		return distr(gen);// Return the random number
	};
};// End of GameStructure class

class Vehicle : public GameStructure
{
public:
	string vehicleMake;
	bool vehicleModified;// Has the vehicle been modified?
	bool tiresInstalled;
	bool superchargerInstalled;
	string vehicledescription = "";
	int horsepower = 0;
	double traction = 0.f;
	AddonsInstalled addons;

	Vehicle()
	{
		/* Init vars */
		vehicleMake = "";
		vehicleModified = false;
		tiresInstalled = false;
		superchargerInstalled = false;
	}  

	int RandomRoll() override
	{
		int roll = GameStructure::RandomRoll();// Call to function in parent class		 
		return roll;
	}
		
	vector<string> ReadAllFromFile()
	{
		/* Vars */
		vector<string> allLines;
		int count = 1;
		string dataFound;
		string line;
		ifstream infile(fileName);
		/* End vars */
		 
		while (getline(infile, line))// Parse file line by line
		{
			istringstream iss(line);// get the line		 
			//cout << "Entry " << count << ": " << line << endl;
			allLines.push_back(line);
			count++;// increment the entry number count
		}
		return allLines;
	}// End ReadAllFromFile

	void AppendToFile(string stringToWrite)
	{
		cout << "Writing data to file..." << endl;
		ifstream file(fileName);// var
		ofstream myfile;// var
		myfile.open(fileName, ios_base::app);// Open file for append	
		if (file.peek() == ifstream::traits_type::eof())// Check for empty file
		{
			// Empty File
			myfile << stringToWrite;// Append new line
		}
		else
		{
			// File has data, so add newline
			myfile << "\n" << stringToWrite;// Append new line
		}
		myfile.close();// Close file
	}// End AppendToFile
	 
	string SearchFileForString(string stringToFind)
	{
		/* vars */
		ifstream input;
		size_t pos;
		string line;
		int count = 1;
		/* end vars */

		input.open(fileName);// Open file
		if (input.is_open())// Did it open?
		{
			while (getline(input, line))// While file is open
			{
				pos = line.find(stringToFind);// Search
				if (pos != string::npos) // string::npos is returned if string is not found
				{
					//cout << "Found!";				 
					return line;
				}
			}
		}
		return "";
	}// End SearchFileForString

	//vector<string> ReadFromSpeedShopFile()
	//{
	//	vector<string> allVehicleEntries;
	//	// Read all lines
	//	// Is there a Chevy entry?
	//		// Get all add-ons for Chevy
	//		// if (chevyEntries != nullptr)
	//		// {
	//		//		chevy.vehicleModified = true;
	//		// }
	//		// else
	//		// {
	//		//		chevy.vehicleModified = false;
	//		// }
	//	// Is there a Dodge entry?
	//		// Get all add-ons for Dodge
	//		// if (dodgeEntries != nullptr)
	//		// {
	//		//		dodge.vehicleModified = true;
	//		// }
	//		// else
	//		// {
	//		//		dodge.vehicleModified = false;
	//		// }
	//	return allVehicleEntries;
	//}

	AddonsInstalled OpenSpeedShop(vector<string> entries, string vehicle)// Open the speed shop and populate the performance add-ons already on vehicle
	{
		 
		/* Determine if there are mods already installed */
		tiresInstalled = false;// Init vars
		superchargerInstalled = false;// Init vars

		 cout << vehicle << " Speed Shop" << " - Make a selection to continue." << endl;
		 for (string entry : entries)// Loop through the passed in entries
		 {
			size_t pos = entry.find("tires");// Search for the word tires in entry
			if (pos != string::npos)// found?
			{
				tiresInstalled = true;
				if (vehicle == "Chevy")
				{
					addons.chevyTires = true;
				}
				if (vehicle == "Dodge")
				{
					addons.dodgeTires = true;
				}
			}			 

			size_t pos2 = entry.find("supercharger");// Search for the word supercharger in entry
			if (pos2 != string::npos)// found?
			{
				superchargerInstalled = true;
				if (vehicle == "Chevy")
				{
					addons.chevySupercharger = true;
				}
				if (vehicle == "Dodge")
				{
					addons.dodgeSupercharger = true;
				}
			}			 
		 }

		/* Speed Shop menu */
		int choice = speedShopMenu(tiresInstalled, superchargerInstalled);
		while (choice != 3)
		{
			if (choice == 1)
			{
				// Add wider tires
				cout << "You added wider tires to your " << vehicle << endl << endl;
				tiresInstalled = true;				
				AppendToFile(vehicle + " tires");// Write line to file
				if (vehicle == "Chevy")
				{
					addons.chevyTires = true;
				}
				if (vehicle == "Dodge")
				{
					addons.dodgeTires = true;
				}
			} // end if choice == 1
			else if (choice == 2)
			{
				cout << "You added a supercharger to your " << vehicle << endl << endl;
				superchargerInstalled = true;				
				AppendToFile(vehicle + " supercharger");// Write line to file
				if (vehicle == "Chevy")
				{
					addons.chevySupercharger = true;
				}
				if (vehicle == "Dodge")
				{
					addons.dodgeSupercharger = true;
				}
			}// end if choice == 2
			else if (choice == 3)
			{
				// Exit to main menu
				choice = mainMenu();
			};// end if choice == 3
			cout << endl;
			choice = speedShopMenu(tiresInstalled, superchargerInstalled);
		};// End While 
		return addons;		
	}

	AddonsInstalled OpenSpeedShop(string vehicle)// Open the speed shop with no add-ons installed
	{	 
		cout << vehicle << " Speed Shop" << " - Make a selection to continue." << endl;
		/* Speed Shop menu */
		int choice = speedShopMenu(0, 0);
		while (choice != 3)
		{
			if (choice == 1)
			{
				// Add wider tires
				cout << "You added wider tires to your " << vehicle << endl << endl;
				tiresInstalled = true;
				AppendToFile(vehicle + " tires");// Write line to file
				if (vehicle == "Chevy")
				{
					addons.chevyTires = true;
				}
				if (vehicle == "Dodge")
				{
					addons.dodgeTires = true;
				}
			} // end if choice == 1
			else if (choice == 2)
			{
				// Add supercharger
				cout << "You added a supercharger to your " << vehicle << endl << endl;
				superchargerInstalled = true;
				AppendToFile(vehicle + " supercharger");// Write line to file
				if (vehicle == "Chevy")
				{
					addons.chevySupercharger = true;
				}
				if (vehicle == "Dodge")
				{
					addons.dodgeSupercharger = true;
				}

			}// end if choice == 2
			else if (choice == 3)
			{
				// Exit to main menu
				choice = mainMenu();
			};// end if choice == 3
			cout << endl;
			choice = speedShopMenu(tiresInstalled, superchargerInstalled);
		};// End While	
		return addons;
	}
};// End of Vehicle class

class Chevy : public Vehicle
{
private:
	string vehicledescription = "The Chevy has great tire traction but not as much horsepower as the Dodge.";
	 int horsepower = 650;
	 double traction = 0.3f;
public:
	string GetVehicleDescription()
	{
		return vehicledescription;
	}
	int GetVehicleHorsepower()
	{
		return horsepower;
	}
	void SetVehicleHorsepower(int hp)
	{
		horsepower = hp;
	}
	double GetVehicleTraction()
	{
		return traction;
	}
	void SetVehicleTraction(double tr)
	{
		traction = tr;
	}
};// End of Chevy Class

class Dodge : public Vehicle
{
private:
	string vehicledescription = "The Dodge has great horsepower but not as much tire traction as the Chevy.";
	int horsepower = 845;
	double traction = 0.1f;

public:
	string GetVehicleDescription()
	{
		return vehicledescription;
	}
	int GetVehicleHorsepower()
	{
		return horsepower;
	}
	void SetVehicleHorsepower(int hp)
	{
		horsepower = hp;
	}
	double GetVehicleTraction()
	{
		return traction;
	}
	void SetVehicleTraction(double tr)
	{
		traction = tr;
	}
};// End of Dodge Class

int main()
{
	/* Class declarations */
	GameStructure gameStructure;
	Vehicle vehicle;
	Chevy chevy;
	Dodge dodge;
	AddonsInstalled addons;

	/* Intro */
	cout << "Welcome to the Drag Racing capital of the Hello World!" << endl << endl;
	cout << "Please choose a menu option to continue" << endl;
	
	/* Main menu */
	int choice = mainMenu();
	while (choice != 8)
	{
		if (choice == 1)
		{
			 // Rules
			 gameStructure.Rules(chevy.GetVehicleDescription(), dodge.GetVehicleDescription());// Display rules and pass in vehicle descriptions
		} // end if choice == 1
		else if (choice == 2)
		{
			// Race! 
			/* Get reaction time for each vehicle */
			int roll = vehicle.RandomRoll();// Get Chevy reaction time
			double reactionTime1 = static_cast<double>(roll) / 1000;
			cout << "Lane 1: Chevy reaction time: " << reactionTime1 << endl;
			roll = vehicle.RandomRoll();// Get Dodge reaction time
			double reactionTime2 = static_cast<double>(roll) / 1000;
			cout << "Lane 2: Dodge reaction time: " << reactionTime2 << endl;

			 
			if (addons.chevyTires == true)
			{
				double tr = chevy.GetVehicleTraction();
				cout << "Current Chevy tire traction: " << tr << endl;
				tr = tr += chevyWiderTireTraction;
				cout << "NEW Chevy tire traction: " << tr << endl;
			}

			if (addons.dodgeTires == true)
			{
				double tr = dodge.GetVehicleTraction();
				cout << "Current Dodge tire traction: " << tr << endl;
				tr = tr += dodgeWiderTireTraction;
				cout << "NEW Dodge tire traction: " << tr << endl;
			}

			if (addons.chevySupercharger == true)
			{
				double tr = chevy.GetVehicleHorsepower();
				cout << "Current Chevy horsepower: " << tr << endl;
				tr = tr += chevySuperchargerHP;
				cout << "NEW Chevy horsepower: " << tr << endl;
			}

			if (addons.dodgeSupercharger == true)
			{
				double tr = dodge.GetVehicleHorsepower();
				cout << "Current Dodge horsepower: " << tr << endl;
				tr = tr += chevySuperchargerHP;
				cout << "NEW Dodge horsepower: " << tr << endl;
			}


			// TODO: Finish all race requirements, timers and threading to start race

		}// end if choice == 2
		else if (choice == 3)
		{	
			// Chevy Speed Shop
			vehicle.tiresInstalled = false;// Init vars
			vehicle.superchargerInstalled = false;// Init vars
			vehicle.vehicleMake = "Chevy";// Specify vehicle make			 			
			vector<string> chevyEntries;// All Chevy entries in file				 
			 
			vector<string> allVehicleEntries = vehicle.ReadAllFromFile();// Get all entries in file
			for (string entry : allVehicleEntries)// Loop
			{
				if (entry.find("Chevy") != string::npos)// Was "Chevy" found in the string on that line?
				{
					chevyEntries.push_back(entry);// Add line to the chevyEntries array
					chevy.vehicleModified = true;// Vehicle was modified
				}				 			
			}

			if (chevy.vehicleModified == true)// Has the vehicle been modified and saved to file?
			{
				addons = vehicle.OpenSpeedShop(chevyEntries, vehicle.vehicleMake);// Open Chevy speed shop with appropriate performance add-ons populated				 
			}
			else
			{
				addons = vehicle.OpenSpeedShop(vehicle.vehicleMake);// Open Chevy speed shop with no performance add-ons already installed
			}
		}// end if choice == 3
		else if (choice == 4)
		{
			// Dodge Speed Shop	 
			vehicle.tiresInstalled = false;// Init vars
			vehicle.superchargerInstalled = false;// Init vars
			vehicle.vehicleMake = "Dodge";// Specify vehicle make			 			
			vector<string> dodgeEntries;// All Dodge entries in file				 

			vector<string> allVehicleEntries = vehicle.ReadAllFromFile();// Get all entries in file
			for (string entry : allVehicleEntries)// Loop
			{
				if (entry.find("Dodge") != string::npos)// Was "Dodge" found in the string on that line?
				{
					dodgeEntries.push_back(entry);// Add line to the DodgeEntries array
					dodge.vehicleModified = true;// Vehicle was modified
				}				 
			}

			if (dodge.vehicleModified == true)// Has the vehicle been modified and saved to file?
			{
				addons = vehicle.OpenSpeedShop(dodgeEntries, vehicle.vehicleMake);// Open Dodge speed shop with appropriate performance add-ons populated
			}
			else
			{
				addons = vehicle.OpenSpeedShop(vehicle.vehicleMake);// Open Dodge speed shop with no performance add-ons already installed
			}
		}// end if choice == 4
		else if (choice == 5)
		{
			/* Load Chevy profile */			
			vector<string> vStr = vehicle.ReadAllFromFile();// Read all items in file
			vector<string> chevyMods;
			for (string str : vStr)// Loop through all entries
			{
				size_t pos = str.find("Chevy");// Search for the word Chevy in entry
				if (pos != string::npos)// found?
				{					 
					 chevy.vehicleModified = true;	
					 chevyMods.push_back(str);
				}				
				
				if (chevy.vehicleModified == true)
				{
					addons = vehicle.OpenSpeedShop(chevyMods, "Chevy");// Open Chevy speed shop with appropriate performance add-ons populated
				}
			}			 
		}// end if choice == 5
		else if (choice == 6)
		{
			/* Load Dodge profile */			
			vector<string> vStr = vehicle.ReadAllFromFile();// Read all items in file
			vector<string> dodgeMods;
			for (string str : vStr)// Loop through all entries
			{
				size_t pos2 = str.find("Dodge");// Search for the word Dodge in entry
				if (pos2 != string::npos)// found?
				{
					dodge.vehicleModified = true;	
					dodgeMods.push_back(str);
				}

				if (dodge.vehicleModified == true)
				{
					addons = vehicle.OpenSpeedShop(dodgeMods, "Dodge");// Open Dodge speed shop with appropriate performance add-ons populated
				}
			}			 
		}// end if choice == 6		
		else if (choice == 7)
		{
			// Show best runs

		}// end if choice == 7
		else if (choice == 8)
		{
			// Exit game			 
		}
		else
		{
			cout << "Invalid choice" << endl;// Exception handling: This message will show if a non-listed number is entered
		};// end else if		
		cout << endl;
		choice = mainMenu();
	}; // end while

	/* Get main thread ID */
	auto myid = this_thread::get_id();
	stringstream ss;
	ss << myid;
	string idString = ss.str();
	mainThreadID = idString;
	cout << "The main base thread ID is: " << mainThreadID << endl << endl;// Gets current thread ID
	system("pause");
}