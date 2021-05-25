#
# Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
# an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
#
# This software, including source code, documentation and related
# materials ("Software") is owned by Cypress Semiconductor Corporation
# or one of its affiliates ("Cypress") and is protected by and subject to
# worldwide patent protection (United States and foreign),
# United States copyright laws and international treaty provisions.
# Therefore, you may use this Software only as provided in the license
# agreement accompanying the software package from which you
# obtained this Software ("EULA").
# If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
# non-transferable license to copy, modify, and compile the Software
# source code solely for use in connection with Cypress's
# integrated circuit products.  Any reproduction, modification, translation,
# compilation, or representation of this Software except as specified
# above is prohibited without the express written permission of Cypress.
#
# Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
# reserves the right to make changes to the Software without notice. Cypress
# does not assume any liability arising out of the application or use of the
# Software or any product or circuit described in the Software. Cypress does
# not authorize its products for use in any products where a malfunction or
# failure of the Cypress product may reasonably be expected to result in
# significant property damage, injury or death ("High Risk Product"). By
# including Cypress's product in a High Risk Product, the manufacturer
# of such system or application assumes all risk of such use and in doing
# so agrees to indemnify Cypress against all liability.
#
package parseCLI;

use strict;
use warnings;

use generaldefines;
#use Data::Dumper;

my @errorMessages   = ();
my @unmatchedParams = ();
my @flagsSet        = ();

#-------------------------------------------------------------------------------
# Function     : parseCommandLineArguments
#
# Description  : Takes in a list of valid command line arguments paired with default
#       values, and returns a hash mapped to the cl arguments containing their
#       values. Expects that commands will have their arguments in the same index.
#           ie -f=4350_ABURST_TR_1 NOT -f 4350_ABURST_TR_1.
#       Multiple values are accepted and an ARRAY is created
#           ie -f=val1,val2
#       To determine if an array is used:
#         if (ref $hash{"b"} eq 'ARRAY')
#         {
#             print "it's an array reference!";
#         }
#
#       Flags are determined by not having a '=' on the end. In the case of such a
#       parameter the default value is assumed to be a boolean and is flipped.
#
# Parameters   : Ref to List of valid flags, Ref to list of arguments, boolean which
#       controls whether or not no flag match is considered an error.
#
# Returns      : ERROR_GENERAL if error encountered, SUCCESS if no error
#-------------------------------------------------------------------------------
sub parseCommandLineArguments
{
    my ($flagsHash, $cLA, $noMatchError) = @_;

    # Make it easier to iterate over
    my @cmdLineArgs = @$cLA;

    #print Dumper(@cmdLineArgs);

    # This will hold any error types
    my $err = SUCCESS;
    $noMatchError = TRUE unless (defined($noMatchError));

    my @keys = keys(%$flagsHash);

    # Re-init our history
    @errorMessages   = ();
    @unmatchedParams = ();
    @flagsSet        = ();

    foreach my $param (@cmdLineArgs)
    {
        my $foundMatch = 0;

        foreach my $key (@keys)
        {
            foreach my $subkey (split(/, */, $key))
            {
                if ($param =~ /^$subkey(.*)/)
                {
                    my $value   = $1;
                    $foundMatch = 1;

                    # Check for parameters with values
                    if ($subkey =~ /=$/)
                    {
                        # Store off the value ... Create array if multiple values
                        if ($value =~ /,/)
                        {
                            # Multiple values, split the input string components into an array,
                            # then make a copy of the array and put it into the hash.
                            my @vals = split(/,/,$value);
                            $flagsHash->{$key} = [@vals];
                        }
                        else
                        {
                            # Single Value
                            $flagsHash->{$key} = $value;
                        }
                        push(@flagsSet, $key); # Keep track of this

                        # Handle errorMessages
                        if ('' eq $value)
                        {
                            $err = ERROR_GENERAL;
                            push(@errorMessages, "NO PARAMETER FOR $subkey");
                        }
                    }
                    else
                    {
                        # Handle flag parameters, these can only be TRUE/FALSE.
                        # Flip the default value
                        if (defined($flagsHash->{$key}))
                        {
                            $flagsHash->{$key} = !$flagsHash->{$key};
                        }
                        else
                        {
                            # No value defined, just set the flag to true
                            $flagsHash->{$key} = TRUE;
                        }
                        push(@flagsSet, $key); # Keep track of this
                    }

                    last;
                }
            }
        }

        if (!$foundMatch)
        {
            $err = ERROR_GENERAL if ($noMatchError);
            push(@errorMessages, "NO MATCH FOR $param");
            push(@unmatchedParams, $param); # Put the unmatched flag into our list
        }
    }

    return $err;
}

#-------------------------------------------------------------------------------
# Function     : getErrors
#
# Description  : Returns a list containing errorMessages encountered.
#
# Parameters   : N/A
#
# Returns      : List containing all errorMessages encountered
#-------------------------------------------------------------------------------
sub getErrors
{
    return @errorMessages;
}

#-------------------------------------------------------------------------------
# Function     : getUnmatchedParams
#
# Description  : Returns a list of parameters which were not matched against the
#       flag hash.
#
# Parameters   : N/A
#
# Returns      : List of unmatched params. Raw from ARGV
#-------------------------------------------------------------------------------
sub getUnmatchedParams
{
    return @unmatchedParams;
}

#-------------------------------------------------------------------------------
# Function     : getFlagsSet
#
# Description  : Returns a list of flags which were set. This list may contain
# duplicates if multiple equal flags were used.
#
# Parameters   : N/A
#
# Returns      : List containing all flags set. Only flag names, no values.
#-------------------------------------------------------------------------------
sub getFlagsSet
{
    return @flagsSet;
}

#-------------------------------------------------------------------------------
# Function     : wasFlagSet
#
# Description  : Returns TRUE if given flag was set, FALSE otherwise.
#
# Parameters   : Flag to check
#
# Returns      : TRUE if set
#-------------------------------------------------------------------------------
sub wasFlagSet
{
    my ($flag) = @_;

    if (!defined($flag))
    {
        return FALSE;
    }
    foreach my $cmdLineFlag (@flagsSet)
    {
        if ($cmdLineFlag =~ /$flag/)
        {
            return TRUE;
        }
    }

    return FALSE;
}

#-------------------------------------------------------------------------------
# Function     : printErrors
#
# Description  : Prints the encountered errorMessages to stdout
#
# Parameters   : N/A
#
# Returns      : N/A
#-------------------------------------------------------------------------------
sub printErrors
{
    foreach (@errorMessages)
    {
        print "CLI ERROR : $_\n";
    }
}

#-------------------------------------------------------------------------------
# Function     : lineupBasedOn
#
# Description  : Lines up text strings based on the given separator. Enforces that
#       each line is at most maxWidth. If the line is longer than maxWidth we
#       split it and indent by the indent level based on separator.
#           If max width is 0, skips max width enforcement.
#
# Parameters   : Separator, max line width, list of strings to line up
#
# Returns      : List of lined up strings
#-------------------------------------------------------------------------------
sub lineupBasedOn
{
    my ($separator, $maxWidth, @lines) = @_;

    my @formatted = ();
    my $longest   = 0;
    my $prefix    = '';

    # Find the longest
    foreach (@lines)
    {
        if (/(.*?)$separator/)
        {
            my $tempLen = length($1);

            if ($tempLen > $longest)
            {
                $longest = $tempLen;
            }
        }
    }

    # Make the prefix
    for (my $var = 0; $var <= $longest + length($separator); $var++)
    {
        $prefix .= ' ';
    }

    # Go ahead and format all lines
    foreach my $str (@lines)
    {
        my $hasSeparator = FALSE;

        # Line 'em up, only lines that have the separator
        if ($str =~ /(.*?)$separator/)
        {
            $hasSeparator = TRUE;

            my $len = length($1);

            # Doing the actual lining up
            while ($len < $longest)
            {
                $str =~ s/$separator/ $separator/;

                $str =~ /(.*?)$separator/;
                $len = length($1);
            }
        }

        # Enforce line width
        while ($maxWidth && length($str) > $maxWidth)
        {
            my $temp      = substr($str, 0, $maxWidth);
            my $leftovers = undef;

            # We don't want to split mid-word, so go to the closest space to the split
            ($temp, $leftovers) = ($temp =~ /(.*) (.*?)$/);

            push(@formatted, $temp."\n");

            # Make sure we indent the overflow
            $str = $prefix.$leftovers.substr($str, $maxWidth, length($str));
        }

        # Make sure all strings have a newline
        if ($str !~ /\n$/)
        {
            $str .= "\n";
        }

        push(@formatted, $str);
    }

    return @formatted;
}

1;
__END__
