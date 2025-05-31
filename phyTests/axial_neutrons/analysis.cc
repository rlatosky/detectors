#include <cstdlib>

// ===== Hipo headers =====
#include <reader.h>
#include <writer.h>
#include <dictionary.h>

#include <TH1D.h>
#include <TH2D.h>
#include <TGraph.h>
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>

#include <iostream>
using namespace std;


int main(int argc, char** argv) {
	// exit with warning if first argument is not given or if it's not an hipo file
	if (argc < 6) {
		cout << " *** please provide a file name, particle, and bins" << endl;
		exit(0);
	}

	const double r2d = TMath::RadToDeg();

	const int neutron_ID = 2112;
	const int proton_ID = 2212;
	const int electron_ID = 11;
	const int positron_ID = -11;
	double    zpos1      = 536.0;
	double    zpos2      = 2536.0;
	double    Mn          = 939.565; // MeV
	//double    M          = 938.272; // MeV
	double    ene_cut    = 3;
	const double PROTONMASS = 928.272; // MeV
	const double NEUTRONMASS = 939.565;
	const double ELECTRONMASS = 0.510998;
	const double PHOTONMASS = 0;

	hipo::reader reader;
	reader.open(argv[1]);

	hipo::dictionary factory;
	reader.readDictionary(factory);
	factory.show();

	hipo::event event;
	int         evCounter = 0;

	hipo::bank bMCTrue(factory.getSchema("MC::True"));
	hipo::bank fluxADC(factory.getSchema("FLUX::adc"));	// get flux entry

	double M;
	cout << argv[2][0] << endl;
	switch (char(argv[2][0])) {	// first letter of second parameter since switches are for ints/chars
		case 'p':
			M = PROTONMASS;
			cout << "Mass set to proton mass..." << endl;
			break;
		case 'n':
			M = NEUTRONMASS;
			cout << "Mass set to neutron mass..." << endl;
			break;
		case 'e':
			M = ELECTRONMASS; // MeV
			cout << "Mass set to electron mass..." << endl;
			break;
		case 'g':
			M = 0; // MeV
			cout << "Mass set to photon mass..." << endl;
			break;
		default:
			M = NEUTRONMASS; // default neutron
			cout << "Mass set to neutron mass..." << endl;
			break;
	}
	std::string fileSavestr = argv[2];
	fileSavestr += argv[3];
	fileSavestr += "-KE.root";
	const char * fileSave = fileSavestr.c_str();
	auto       file_out = new TFile(fileSave, "Recreate");
	auto tree = new TTree(fileSave,"Tree");

	std::string hTotEdepTitlestr = "Total ";
	hTotEdepTitlestr += argv[2];
	hTotEdepTitlestr += " E_dep - ";
	hTotEdepTitlestr += argv[3];
	hTotEdepTitlestr += " MeV";
	const char * hTotEdepTitle = hTotEdepTitlestr.c_str();
	const int numOfHistBins = atoi(argv[4]);
	const int maxHistNum = atoi(argv[5]);
	TH1D h_totedep("h_totedep", hTotEdepTitle, numOfHistBins, 0, maxHistNum);

	h_totedep.GetXaxis()->SetTitle("E_dep (MeV)");
	h_totedep.GetYaxis()->SetTitle("Events");

	std::string hEffEdepTitlestr = "Total ";
	hEffEdepTitlestr += argv[2];
	hEffEdepTitlestr += " E_dep - ";
	hEffEdepTitlestr += argv[3];
	hEffEdepTitlestr += " MeV";
	const char * hEffEdepTitle = hEffEdepTitlestr.c_str();
	TH1D h_effedep("h_effedep", hEffEdepTitle, numOfHistBins, 0, maxHistNum);

	h_effedep.GetXaxis()->SetTitle("E_dep (MeV)");
	h_effedep.GetYaxis()->SetTitle("Events (with cut)");

	TGraph g_avgTvsAvgZ(10000);
	g_avgTvsAvgZ.SetTitle("AvgT vs AvgZ; avgT; avgZ");
	try {
		while (reader.next() == true) {
			reader.read(event);

			evCounter = evCounter + 1;

			if (evCounter % 1000 == 0) { cout.flush() << "Processed " << evCounter << " events \r"; }
			event.getStructure(bMCTrue);
			event.getStructure(fluxADC);

			int nPart = bMCTrue.getRows();

			// Get total number of Edep per event
			float edep_sum;
			float eff_edep_sum;
			float e_threshold = 100;
			//cout << size(bMCTrue.getFloat("totEdep")) << endl;
			for (auto i: bMCTrue.getFloat("totEdep")) {
				int mpid = bMCTrue.getInt("mpid")[i];
				int pid = bMCTrue.getInt("pid")[i];
				edep_sum += i; // uncomment if you want all secondaries
//				// Let's filter out the particles
//				if (pid == proton_ID && M == PROTONMASS) { //proton
//					//cout << "Particle ID:" << pid << endl;
//					edep_sum += i;
//				} else if (pid == neutron_ID && M == NEUTRONMASS) {
//					edep_sum +=i;
//				} else if (pid == electron_ID && M == ELECTRONMASS) {
//					edep_sum +=i;
//				} else if (pid == positron_ID && M == ELECTRONMASS) {
//					edep_sum +=i;
//				}
			}
			int eff_events = 0;
			if (edep_sum > e_threshold) {
				eff_events += 1; // one event that reached threshold
				h_effedep.Fill(edep_sum);
			}
			h_totedep.Fill(edep_sum);
			edep_sum = 0;
			eff_edep_sum = 0;

//			float avgT_sum;
//			float avgZ_sum;
//			for (auto i: bMCTrue.getFloat("avgT")) {	// Equals 10,000 entries
//				avgZ_sum += bMCTrue.getFloat("avgZ")[i];
//				int mpid = bMCTrue.getInt("mpid")[i];
//				int pid = bMCTrue.getInt("pid")[i];
//				// Let's filter out the particles
//				if (pid == proton_ID && M == PROTONMASS) { //proton
//					//cout << "Particle ID:" << pid << endl;
//					avgT_sum += i;
//				} else if (pid == neutron_ID && M == NEUTRONMASS) {
//					avgT_sum +=i;
//				} else if (pid == electron_ID && M == ELECTRONMASS) {
//					avgT_sum +=i;
//				} else if (pid == positron_ID && M == ELECTRONMASS) {
//                    avgT_sum += i;
//                }
//			}
//			g_avgTvsAvgZ.AddPoint(avgT_sum, avgZ_sum);
			double Edep_slab1 = 0;
			double Edep_slab2 = 0;
//			vector<double> Edep_slab;
			//vector<double> tot_Edep;
//			vector<double> zpos;
			int numOfSlabs = 21;
			int gap = 3;
			int leadThickness = 5; // mm
			int slab1Thickness = 6;
			int slab2Thickness = 5;

			double x[nPart], y[nPart];

			for (int ip = 0; ip < nPart; ip++) {

				double avgZ = bMCTrue.getFloat("avgZ", ip);
				double Edep = double(bMCTrue.getFloat("totEdep", ip));
				double identifier = int(fluxADC.getInt("component", ip));
				//h_totedep.Fill(Edep);


				//tot_Edep.push(Edep);
                double total_edep_layer[numOfSlabs];
				// iterate through zpos and edep_slab arrays
				for (int n = 1; n < numOfSlabs; n++) {
                    if (identifier == n) {
                        total_edep_layer[n] += Edep;
                    }
//					// create zpos
//					if (n == 1 || n == 2) {
//						Edep_slab.push_back(0);
//						zpos.push_back(zpos[n-1] + gap + slab1Thickness + gap + leadThickness + gap);
//						//cout << "Zpos: " << zpos[n] << endl;
//						//cout << "Edep_slab: " << Edep_slab[n] << endl;
//						if (avgZ < zpos[n] - slab1Thickness) { Edep_slab[n] += Edep; }
//						z
//					} else {
//						Edep_slab.push_back(0);
//						zpos.push_back(zpos[n-1] + gap + leadThickness + gap + slab2Thickness);
//						//cout << "Zpos: " << zpos[n] << endl;
//						//cout << "Edep_slab: " << Edep_slab[n] << endl;
//						if (avgZ < zpos[n] - slab2Thickness) { Edep_slab[n] += Edep; }
//					}
				}
				// cout << "Edep: " << Edep << endl;
				// cout << "avgZ: " << avgZ << endl;
				// // in slab1
				// if (avgZ < zpos1 - 4) { Edep_slab1 += Edep; }
				// // in slab2
				// if (avgZ > zpos1 + 4 && avgZ < zpos2 - 4) { Edep_slab2 += Edep; }
			}

		}
	}
	catch (const char msg)
	{
		cerr << msg << endl;
	}
	//tree->Branch("h_totedep", &h_totedep, 32000, 0);
	gDirectory->Write();
	return 0;
}
