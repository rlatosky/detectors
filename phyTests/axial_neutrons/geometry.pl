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

    my $slab_side = 22.816;
    my $slab_width = 22.816;      # 228.16 mm wide slabs

    my $lead_side = 22.8;

    my $slabs_thickness1 = 0.5/2; # This is semi-length
    my $slabs_thickness2 = 0.6/2;
    my $slabs_difference = 0.1/2;

    my $lead_thickness = 0.5/2; # This is semi-length

    my $gap = .3; # micrometer
    my $slab_zpos = 0;  # Initial slab z-pos
    my $lead_zpos = $slabs_thickness2 + $gap; # Initial lead z-pos

    my @parts = (1..21);

    for(@parts) {
        my %detector = init_det();

        my $slabs_thickness = $slabs_thickness2 if ($_ == 1 || $_ == 2);
        $slabs_thickness = $slabs_thickness1 if ($_ != 1 && $_ != 2);

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
        #$detector{"identifiers"} = "id manual s$_"; # Comment for now, remove comment later
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
}

1;


