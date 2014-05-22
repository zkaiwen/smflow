#/usr/bin/perl

# @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@
# @
# @      DOT2G - Converts a dot file to a graph file
# @      
# @      @AUTHOR:Kevin Zeng
# @      Copyright 2011 – 2013 
# @      Virginia Polytechnic Institute and State University
# @
# @#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@


# Checks to see if there are arguments
if(@ARGV < 2){
	print "################################################################################\n\n";
	print "[ERROR] -- Not enough arguments..exiting\n\n";
	print "perl dot2g -f <dot file>\n";
	print "perl dot2g -d <directory>\n";
	print "  -d -- convert entire directory\n\n";
	print "################################################################################\n";
	exit;
} 


print "================================================================================\n";
print " [*]\tBegin Dot to G translation\n";
print "================================================================================\n";
if($ARGV[0] =~ m/-d/)
{
	$dirPath = $ARGV[1];
	# Get list of files in directory path
	@dirFiles = <$dirPath*.dot>;
	print "[dot2g] -- Searching Directory: " . $dirPath . "\n";
	
	# Convert each dot file to a g file
	$numCircuits = 0;
	foreach(@dirFiles){
		$fileName = $_;
		print " -- File: " . $fileName . "\n";
		&parse();
		$numCircuits = $numCircuits + 1;
	}
	print "\nNumber of circuits processed: " . $numCircuits . "\n";
}
else
{
	$fileName = $ARGV[1];
	print "[dot2g] -- Converting File: " . $fileName. "\n";
	&parse();
}

print "COMPLETED\n\n";





sub parse{
	@numOutput = ();
	@numInput = ();
	@inputs = ();
	@outputs = ();
	@type = ();
	
	%vertex = ();
	%inputNodes = ();
	%lutmap = ();

	$index = 0;
	$numInputNodes = 0;
	$newDot = "digraph graph{\n";
	
	#Open dot File to parse 
	open(fileStream, $fileName);
	@file = <fileStream>;


	foreach (@file)
	{
		#Only look at lines that have connections information
		if(/\$/){
			@lutarray = split(/\$/);
			$lutname = substr($lutarray[0], 1, index($lutarray[0],"[")-1);
			#print "LUT INIT" . $lutarray[1] ."\n";
			#print "LUT NAME|" . $lutname . "|\n";
			$lutmap{$lutname} = $lutarray[1];

			

		}		
		elsif(/->/)
		{
			#Get rid of white space characters
			s/[\t\n ]//g;
			
			#Get inputs and outputs
			@inout = split(/->/);
			@gateOutPort = split(':', $inout[0]);
		
			#Checks source to see if a label has been assigned to the component
			if(exists($vertex{$gateOutPort[0]}) == false)
			{
				#Give the vertex a label and set up input and outputs with the label
				$vertex{$gateOutPort[0]} = $index;
				$numInput[$index] = 0; $numOutput[$index] = 0;
				$index = $index + 1;

			}
			
			#Get rid of port information
			@gateInPort = split(':', $inout[1]);
			#print $gateInPort[0] . " ------ " . $gateInPort[1];
				
			#Checks destination to see if a label has been assigned to the component
			if(exists($vertex{$gateInPort[0]}) == false)
			{
				$vertex{$gateInPort[0]} = $index;
				$numInput[$index] = 0;
				$numOutput[$index] = 0;
				$index = $index + 1;
			}
	
			#Add a connection	
			$i = $vertex{$gateOutPort[0]};
			$o = $vertex{$gateInPort[0]};
		
			
			if($ARGV[0] !~ m/n/)
			{
				@gateNum1 = split("__", $gateOutPort[0]);
				$gateOutPort[0] = $gateNum1[0];
				@gateNum2 = split("__", $gateInPort[0]);
				$gateInPort[0] = $gateNum2[0];
				$cname[$i] = $gateNum1[2];
				$cname[$o] = $gateNum2[2];
				
			}
			#print "\n" . $gateOutPort[0] . " --- " . $gateInPort[0] . "\n";

			$type[$i] = $gateOutPort[0];
			$type[$o] = $gateInPort[0];

			$newDot = $newDot . "\t" . $gateOutPort[0] . "__" . $i . " -> " . $gateInPort[0] . "__" . $o . "\n";
			
			$numOutput[$i] = $numOutput[$i] + 1;
			$outputs[$i] = $o . " " . $gateOutPort[1] . " " . $outputs[$i];
			$numInput[$o] = $numInput[$o] + 1;
			$inputs[$o] =  $i . " " . $gateInPort[1] . " " . $inputs[$o];
			
			if($gateOutPort[0] =~ m/IN/ and $gateOutPort[0] !~ m/INV/ and exists($inputNodes{$i}) == false)
			{
				$inputNodes{$i} = $i;
			}
	
		}
	}

	#Sort the keys based on the vertex labeling
	my @keys = sort { $vertex{$a} <=> $vertex{$b} } keys %vertex;
	
	$c = 0;
	$g = scalar keys %vertex;
	
	if($g != 0)
	{
		$g = $g . "\n" . keys(%inputNodes) . " ";
		$count = 0;
		for my $key (keys %inputNodes)
		{
			$g = $g . $key . " I" . $count . " ";
			$count = $count + 1;
		}

		$g = $g . "\n";
		
		#Concatenate the necessary information for the file 
		#print "KEYS\n";
		foreach my $key (@keys)
		{
			#print $key . "\n";
			$g = $g .  "$vertex{$key} $type[$c] $cname[$c] $numInput[$c] $inputs[$c] $numOutput[$c] $outputs[$c]";
			if(exists($lutmap{$key})){
				#print "EXISTS\n";
				$g = $g . " $lutmap{$key}";
			}
			$g = $g . "\n";
			$c = $c + 1;
			delete $vertex{$key};
		}

		#print $g;

		$fileName =~ s/.dot//g;
		$fileOut = $fileName . ".g";
	
		open(outStream, ">$fileOut");
	
		print outStream $g;
		print " -- * Graph File: " . $fileOut . " EXPORTED\n\n";
		if($ARGV[0] =~ m/-fnp/)
		{
			print $newDot . "}";
		}
	}
	else
	{
		#Delete files that are empty
		unlink($fileName);
	}

}
