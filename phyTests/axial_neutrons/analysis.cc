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

	const double total_events_gemc = 10000.0; // eff_values need to be type float
	const int neutron_ID = 2112;
	const int proton_ID = 2212;
	const int electron_ID = 11;
	const int positron_ID = -11;
	const double PROTONMASS = 928.272; // MeV
	const double NEUTRONMASS = 939.565;
	const double ELECTRONMASS = 0.510998;
	const double PHOTONMASS = 0;

	const char* file = argv[1];
	const char* particle = argv[2];
	const int kin_energy = atoi(argv[3]);
	const int hist_bins_and_xlim = atoi(argv[4]);
	const char* filename_woext = argv[5];

	// Debug
	cout << "Arguments:" << endl;
	cout << "argv[1]:\t" << argv[1] << endl;
	cout << "argv[2]:\t" << argv[2] << endl;
	cout << "argv[3]:\t" << argv[3] << endl;
	cout << "argv[4]:\t" << argv[4] << endl;

	// Initialize hipo library
	hipo::reader reader;
	reader.open(file);
	hipo::dictionary factory;
	reader.readDictionary(factory);
	//factory.show();

	hipo::event event;
	int evCounter = 0;
	hipo::bank bMCTrue(factory.getSchema("MC::True"));
	hipo::bank fluxADC(factory.getSchema("FLUX::adc"));	// get flux entry

	// Initialize graphs
    TGraph g_efficiency(0);

	// Graph and filename options
	const char* file_ext = ".root";
	const char* hist_x_label = "E_dep (MeV)";
	const char* hist_y_label = "Events";
	const char* hist_eff_y_label = "Events (with energy cut)";
	const char* hist_totedep_name = "h_totedep";
	const char* hist_eff_name = "h_effedep";

	const char* eff_graph_name = "efficiency";
	std::string eff_graph_title_str = particle;
	eff_graph_title_str += " - Threshold vs. Efficiency - ";
	eff_graph_title_str += filename_woext;
	eff_graph_title_str += ";Thresholds (MeV);Efficiency (N_det/N_events)";
	const char* eff_graph_title = eff_graph_title_str.c_str();

	std::string h_edep_title_str = particle;
	h_edep_title_str += " E_dep - ";
	h_edep_title_str += filename_woext;
	const char* h_edep_title = h_edep_title_str.c_str();

	// Setup filename for .root file
	std::string file_save_str = filename_woext;
	file_save_str += file_ext;
	const char* file_save = file_save_str.c_str();
	auto file_out = new TFile(file_save, "Recreate");
	auto tree = new TTree(file_save,"Tree");

	double M;
	switch (char(particle[0])) {
	// first letter of second parameter since switches are only for ints/chars
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
		case 'g':	// Photons are called gammas in GEMC
			M = 0; // MeV
			cout << "Mass set to photon mass..." << endl;
			break;
		default:
			M = NEUTRONMASS; // default to neutron
			cout << "Mass set to neutron mass..." << endl;
			break;
	}

	// Set up histograms
	TH1D h_totedep(hist_totedep_name, h_edep_title, hist_bins_and_xlim, 0, hist_bins_and_xlim);
	TH1D h_effedep(hist_eff_name, h_edep_title, hist_bins_and_xlim, 0, hist_bins_and_xlim);

	// Set up histogram axes titles
	h_totedep.GetXaxis()->SetTitle(hist_x_label);
	h_totedep.GetYaxis()->SetTitle(hist_y_label);
	h_effedep.GetXaxis()->SetTitle(hist_x_label);
	h_effedep.GetYaxis()->SetTitle(hist_eff_y_label);

	// The reason for kin_energy + 1 is due to us including 0
	double eff_values[kin_energy+1];
	double e_thresholds[kin_energy+1];
	int events_detected[kin_energy+1];
	for (int a = 0; a < kin_energy+1; a++) {
 		// Initialize
		events_detected[a] = 0;
		e_thresholds[a] = a;
	}

	try {
		while (reader.next() == true) {
			// THIS IS FOR ONE EVENT GOING THROUGH!
			reader.read(event);

			evCounter = evCounter + 1;

			if (evCounter % 1000 == 0) { cout.flush() << "Processed " << evCounter << " events \r"; }
			event.getStructure(bMCTrue);
			event.getStructure(fluxADC);

			int nPart = bMCTrue.getRows(); // Equals number of particles in one event

			// Get total number of Edep per event
			float edep_sum = 0;
			float neutron_edep = 0;

			for (auto i: bMCTrue.getFloat("totEdep")) {
				int mpid = bMCTrue.getInt("mpid")[i];
				int pid = bMCTrue.getInt("pid")[i];
				edep_sum += i; // All secondaries
                if (pid == neutron_ID && M == NEUTRONMASS) {
                	neutron_edep += i;	// Example: only receive neutron energy dep
                }
			}

			// Code to calc efficiency based on different thresholds
			for (int a = 0; a < kin_energy + 1; a++) {
				if (edep_sum >= e_thresholds[a]) {
					// This will count every event detected for each threshold
					// Say the particle has an edep_sum of 50 MeV,
					// it will then count events from 0 to 50 MeV
					events_detected[a]++;
				}
			}

			// Fill in histograms
			h_totedep.Fill(edep_sum);
			if (edep_sum > 100) {
				h_effedep.Fill(edep_sum);
			}

			// Clear edep_sum for next event
			edep_sum = 0;

			const int numOfSlabs = 21;

			for (int ip = 0; ip < nPart; ip++) {

				double avgZ = bMCTrue.getFloat("avgZ", ip);
				double Edep = double(bMCTrue.getFloat("totEdep", ip));
				double identifier = int(fluxADC.getInt("component", ip)); // ID of scintillator

				double total_edep_layer[numOfSlabs];
				// Iterate through zpos and edep_slab arrays
				for (int n = 1; n < numOfSlabs; n++) {
					if (identifier == n) {
						total_edep_layer[n] += Edep; // Get total Edep for layer n
					}
				}

			}

		}
	}
	catch (const char msg)
	{
		cerr << msg << endl;
	}


	int tot_events_detected = 0;

	// After every event is finished processing, find efficiency for each event
	for (int i = 0; i < kin_energy+1; i++) {
		cout << "events_detected[" << i << "]" << " = " << events_detected[i] << endl;
		eff_values[i] = events_detected[i]/total_events_gemc;
		tot_events_detected += events_detected[i];

		// Debugging - Carmack would hate this...
		// and I agree it's bad, so use GDB to test values (don't be me)

		cout << "eff_values[" << i << "]" << " = " << eff_values[i] << endl;

		if (e_thresholds[i] < 2*kin_energy && eff_values[i] != 0) {
			g_efficiency.AddPoint(i, eff_values[i]);
		}
	}
	cout << tot_events_detected << endl;

	g_efficiency.SetTitle(eff_graph_title);
	//Optional: g_efficiency.Fit("gaus");

	// Write Graphs
	g_efficiency.Write(eff_graph_name);

	// Write Histograms
	gDirectory->Write();
	return 0;
}
