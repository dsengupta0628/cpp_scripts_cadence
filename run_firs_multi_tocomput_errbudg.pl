use List::Util qw(max);

#system("rm ./obj/*");
#system("rm ./auxfiles/*");
#system("make");


@sig_files=("signal_blues.txt","signal_classical.txt","signal_country.txt","signal_disco.txt","signal_hiphop.txt","signal_jazz.txt","signal_metal.txt","signal_pop.txt","signal_reggae.txt","signal_rock.txt");

$coeff_file="coeffs_fir3.txt";
$Apx_monte_file="Napx_vals.txt"; #only for the scatter plot- we already have one set of results for blues signal as "filt_signal_blues_0.txt"
$num_alts=1; #uses the 1st among the 100 varieties of each signal genre- used in ASPDAC paper
$sig_len=1000;

my $LEN=@sig_files;
print "$LEN \n";

$i=0;
while($i<$LEN)
{
print "COMMAND_$i---->:   ./do_filter $sig_files[$i] $coeff_file $Apx_monte_file $num_alts $sig_len \n";

#Usage:"<executable> <signal_file> <coefficients_file> <Apx_file> <howmany_alts_of_signal> <signal_length_max>"<<endl;
system("./do_filter $sig_files[$i] $coeff_file $Apx_monte_file $num_alts $sig_len"); 

print "F--I--N--I--S--H--E--D one signal \n";

#Convert the result (each row is for one vector of Napx vals, each column is the filtered signal at a given interval) into a 2D array
$from_file="./auxfiles/filt_signal.txt";
$opfile_name="./data/filt_$sig_files[$i]";
system("cp $from_file $opfile_name");


$i=$i+1;
}

close($opfile1); close($opfile2);
