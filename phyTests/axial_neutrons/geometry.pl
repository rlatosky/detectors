use strict;
use warnings;

our %configuration;

sub make_materials {
    # Scintillator
    my %mat = init_mat();
    $mat{"name"} = "scintillator";
    $mat{"description"} = "slabs scintillator material";
    $mat{"density"} = "1.032";
    $mat{"ncomponents"} = "2";
    $mat{"components"} = "C 9 H 10";
    print_mat(\%configuration, \%mat);

}

sub make_test_geo {

    my $gap = 0.3; # mm
    my $slab_side = 22.816;
    my $slab_width = 22.816;      # 228.16 mm wide slabs

    my $lead_side = 22.8;

    my $slabs_thickness1 = 0.5/2; # This is semi-length
    my $slabs_thickness2 = 0.6/2;
    my $slabs_difference = 0.1/2;

    my $lead_thickness = 0.5/2; # This is semi-length
    #my %detector = init_det();
    my $flead_thickness = 10/2; # 50 mm - This is semi-length
    my %detector = init_det();
    $detector{"name"} = "front_lead_block";
    $detector{"mother"} = "root";
    $detector{"description"} = "Front lead block";
    $detector{"color"} = "aa0000";
    $detector{"style"} = 1;
    $detector{"visible"} = 1;
    $detector{"type"} = "Box";
    $detector{"dimensions"} = "$slab_side*cm $slab_side*cm $flead_thickness*cm";
    $detector{"material"} = "G4_Pb";
    $detector{"pos"} = "0*mm 0*mm 0*cm";
    #$detector{"identifiers"} = "fp1"; # Comment for now, remove comment later
    print_det(\%configuration, \%detector);

    my $fp_thickness = 1.5/2; # 15 mm - This is semi-length
    my $fp_zpos = $flead_thickness + $gap + $fp_thickness;
    %detector = init_det();
    $detector{"name"} = "front_plane";
    $detector{"mother"} = "root";
    $detector{"description"} = "Front plate";
    $detector{"color"} = "2986cc";
    $detector{"style"} = 1;
    $detector{"visible"} = 1;
    $detector{"type"} = "Box";
    $detector{"dimensions"} = "$slab_side*cm $slab_side*cm $fp_thickness*cm";
    $detector{"material"} = "G4_Al";
    $detector{"pos"} = "0*mm 0*mm $fp_zpos*cm";
    #$detector{"identifiers"} = "fp1"; # Comment for now, remove comment later
    print_det(\%configuration, \%detector);


    my $slab_zpos = $fp_zpos + $fp_thickness + $gap;  # Initial slab z-pos
    my $lead_zpos = $slab_zpos + $slabs_thickness2 + $gap; # Initial lead z-pos

    my @parts = (1..21);

    for(@parts) {

        my $slabs_thickness = $slabs_thickness2 if ($_ == 1 || $_ == 2);
        $slabs_thickness = $slabs_thickness1 if ($_ != 1 && $_ != 2);

        %detector = init_det();
        $detector{"name"} = "slab$_";
        $detector{"mother"} = "root";
        $detector{"description"} = "Slab$_ scintillator";
        $detector{"color"} = "f27a9d";
        $detector{"style"} = 1;
        $detector{"visible"} = 1;
        $detector{"type"} = "Box";
        $detector{"dimensions"} = "$slab_side*cm $slab_side*cm $slabs_thickness*cm";
        $detector{"material"} = "scintillator";
        $detector{"pos"} = "0*mm 0*mm $slab_zpos*cm";
        $detector{"sensitivity"} = "flux";
        $detector{"hit_type"} = "flux";
        $detector{"identifiers"} = "id manual $_"; # Comment for now, remove comment later
        print_det(\%configuration, \%detector);

#         %detector = init_det();
#         $detector{"name"} = "flux$_";
#         $detector{"mother"} = "root";
#         $detector{"description"} = "Slab$_ Flux Detector";
#         $detector{"color"} = "ed1845";
#         $detector{"style"} = 1;
#         $detector{"visible"} = 1;
#         $detector{"type"} = "Box";
#         $detector{"dimensions"} = "$slab_side*cm $slab_side*cm $flux_thickness*cm";
#         $detector{"material"} = "G4_Galactic";
#         $detector{"pos"} = "0*mm 0*mm $flux_zpos*cm";
#         $detector{"sensitivity"} = "flux";
#         $detector{"hit_type"} = "flux";
#         #$detector{"identifiers"} = "id manual f$_";
#         print_det(\%configuration, \%detector);

        if($_ != 21) {
            %detector = init_det();
            $detector{"name"} = "lead$_";
            $detector{"mother"} = "root";
            $detector{"description"} = "Lead$_ block";
            $detector{"color"} = "00a552";
            $detector{"style"} = 1;
            $detector{"visible"} = 1;
            $detector{"type"} = "Box";
            $detector{"dimensions"} = "$slab_side*cm $slab_side*cm $lead_thickness*cm";
            $detector{"material"} = "G4_Pb";
            $detector{"pos"} = "0*mm 0*mm $lead_zpos*cm";

            print_det(\%configuration, \%detector);
        }

        $slab_zpos += ($lead_zpos - $slab_zpos) + $gap + $slabs_thickness;
        $slab_width += 1.88; # increase width by 18.8 mm each time
        #Special cases
        #$slab_zpos -= ($lead_thickness + $gap) if ($_ == 21);

        #$slab_zpos = $_*($slabs_thickness2  + $gap + $lead_thickness) if ($_ == 1 || $_ == 21); # 0.05 difference

        $lead_zpos = $slab_zpos + $gap + $lead_thickness;

    }

    my $bp_thickness = 4/2; # 40 mm
    my $bp_zpos = $slab_zpos + $bp_thickness - ($gap + $lead_thickness + $slabs_thickness1);
    %detector = init_det();
    $detector{"name"} = "back_plane";
    $detector{"mother"} = "root";
    $detector{"description"} = "Backplane";
    $detector{"color"} = "2986cc";
    $detector{"style"} = 1;
    $detector{"visible"} = 1;
    $detector{"type"} = "Box";
    $detector{"dimensions"} = "$slab_side*cm $slab_side*cm $bp_thickness*cm";
    $detector{"material"} = "G4_Al";
    $detector{"pos"} = "0*mm 0*mm $bp_zpos*cm";
    #$detector{"identifiers"} = "bp1"; # Comment for now, remove comment later
    print_det(\%configuration, \%detector);


    my $bs_thickness = 10/2;
    my $bs_sides = 6/2;
    my $bs_height = 100/2;
    my $bs_zpos = $bp_zpos + $fp_zpos + $gap + $bs_thickness;
    %detector = init_det();
    $detector{"name"} = "back_scintillator";
    $detector{"mother"} = "root";
    $detector{"description"} = "Back Scintillator";
    $detector{"color"} = "26c98d";
    $detector{"style"} = 1;
    $detector{"visible"} = 1;
    $detector{"type"} = "Box";
    $detector{"dimensions"} = "$slab_side*cm $bs_height*cm $bs_thickness*cm";
    $detector{"material"} = "scintillator";
    $detector{"sensitivity"} = "flux";
    $detector{"hit_type"} = "flux";
    $detector{"pos"} = "0*mm 0*mm $bs_zpos*cm";
    $detector{"identifiers"} = "id manual 22"; # Comment for now, remove comment later
    print_det(\%configuration, \%detector);

}

1;
