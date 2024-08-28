#include <TCanvas.h>
#include <TPolyLine3D.h>
#include <TGeoManager.h>
#include <TGeoVolume.h>
#include <TGeoMatrix.h>
#include <TView.h>

// Function to return the node corresponding to the volume at the point (xi, yi, zi)
TGeoNode* GetNodeAtPoint(Double_t xi, Double_t yi, Double_t zi) {
    // Get the global TGeoManager
    TGeoManager *geom = gGeoManager;
    geom->SetCurrentPoint(xi, yi, zi);
    // Find and return the node at the given point
    TGeoNode *node = geom->FindNode();
    return node; //if not found, node=nullptr !!!
}

void SiliconDetector() {
    // Create a ROOT TGeoManager instance for geometry
    TGeoManager *geom = new TGeoManager("SiliconDetector", "Detector Simulation");

    // Define vacuum material
    TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0, 0, 0);
    TGeoMedium *medVacuum = new TGeoMedium("Vacuum", 1, matVacuum);

    // Define silicon material
    TGeoMaterial *matSi = new TGeoMaterial("Silicon", 28.0855, 14, 2.33);
    TGeoMedium *medSi = new TGeoMedium("Silicon", 1, matSi);

    // Create the top world volume filled with vacuum
    TGeoVolume *top = geom->MakeBox("World", medVacuum, 100, 100, 100);
    geom->SetTopVolume(top);

    // Make the top world volume invisible
    top->SetVisibility(kFALSE);

    // Create a trapezoidal silicon detector
    // Parameters for the trapezoid: dz, theta, phi, h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2
    Double_t dz = 0.1;      // Half length in Z
    Double_t theta = 0.0;   // Polar angle of the line joining the centers of the two faces
    Double_t phi = 0.0;     // Azimuthal angle of the line joining the centers of the two faces
    Double_t h1 = 10.0;     // Half height of the face at -dz
    Double_t bl1 = 8.0;     // Half width of the bottom of the face at -dz
    Double_t tl1 = 12.0;    // Half width of the top of the face at -dz
    Double_t alpha = 0.0;  // Angle with respect to the Y-axis from the center of the face at -dz

    TGeoVolume *detector = geom->MakeTrap("Detector", medSi, dz, theta, phi, h1, bl1, tl1, alpha, h1, bl1, tl1, alpha);
    detector->SetLineColor(kBlue);
    top->AddNode(detector, 1, new TGeoTranslation(0, 0, 0)); // Place the detector at origin

    // Draw the geometry
    geom->CloseGeometry();
    // Example point to check (replace with the actual intersection point)
    Double_t xi = 0.5;
    Double_t yi = 0.5;
    Double_t zi = 3.0;

    // Check if the point is inside the detector
    TGeoNode *node = GetNodeAtPoint(xi, yi, zi);

    if (node != nullptr) {
        TGeoVolume *vol = node->GetVolume();
        std::cout << "The point is inside volume: " << vol->GetName() << std::endl;
    } else {
        std::cout << "The point is not inside any volume." << std::endl;
    }

    TCanvas *c1 = new TCanvas("c1", "Silicon Detector", 800, 600);
    top->Draw("ogl"); // OpenGL view

    // Simulate a particle passing through the detector
    TPolyLine3D *particle = new TPolyLine3D(2); // Create a line with 2 points
    particle->SetPoint(0, 0, 0, -50);  // Start point outside the detector
    particle->SetPoint(1, 0, 0, 50);   // End point outside the detector
    particle->SetLineColor(kRed);
    particle->SetLineWidth(2);
    particle->Draw(); // Draw the particle

    c1->Update();
}

