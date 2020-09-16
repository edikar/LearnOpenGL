#!/usr/bin/perl -w

while(<>){
	$line = $_;

	if($line =~ /^f/){
		@a = split(/ /, $line);
		#printf("$#a: $line");
		if($#a == 3){
			print $line;
		}

		elsif($#a == 4){
			print "f $a[1] $a[2] $a[3]\n";
			print "f $a[1] $a[3] $a[4]";
		}
		else{
			print "error: $#a\n";
			print $line;
			exit(0);
		}
	}else{
		print $line;
	}
}