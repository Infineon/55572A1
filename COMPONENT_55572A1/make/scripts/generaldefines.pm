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

#
#   Abstract:   All common defines are global here..
#
package generaldefines;

use strict;
use warnings;

use Exporter 'import';
our @EXPORT = qw(TRUE   SUCCESS   ERROR_GENERAL   ERROR_FATAL   WINDOWS   LINUX   FALSE
                 printERR  printWARN  printINFO  initDBG  printDBG );

use constant TRUE                     => 1;
use constant FALSE                    => 0;

# Function results
use constant SUCCESS                  => 1000;
use constant ERROR_GENERAL            => 1001;
use constant ERROR_FATAL              => 1002;

# OS Specific Flags
use constant WINDOWS                  => 1;
use constant LINUX                    => 2;

# Any message lower than this level is printed
my $generalVerboseLevel = 1;

#-------------------------------------------------------------------------------
# Function      : printERR, printWARN, printDBG
#
# Description   : Prints message with the filename and line number of where the
#                 call to this routine took place.
#
# Inputs        : Message
#
# Outputs       : Printing of message to the user.
#
# Returns       : N/A
#-------------------------------------------------------------------------------
sub printERR
{
    my ($msg) = @_;

    my ($package, $filename, $line) = caller;

    if (!defined($msg))
    {
        $msg = ' ';
    }
    print "\n      ERROR : ($filename:$line): $msg";
}

sub printWARN
{
    my ($msg) = @_;
    my ($package, $filename, $line) = caller;

    if (!defined($msg))
    {
        $msg = ' ';
    }
    print "\n    WARNING : ($filename:$line): $msg";
}

sub printINFO
{
    my ($msg) = @_;
    my ($package, $filename, $line) = caller;

    if (!defined($msg))
    {
        $msg = ' ';
    }
    print "\n       INFO : ($filename:$line): $msg";
}

sub initDBG
{
    my ($msgDebugLevl) = @_;

    $msgDebugLevl = 0 if (!defined($msgDebugLevl));

    $generalVerboseLevel = $msgDebugLevl;

    printDBG("Debug Level is $msgDebugLevl",1);
}
sub printDBG
{
    my ($msg, $msgDebugLevl) = @_;
    my ($package, $filename, $line) = caller;

    $msgDebugLevl = 2 if !defined($msgDebugLevl);

    if (!defined($msg))
    {
        $msg = ' ';
    }
    if ($msgDebugLevl <= $generalVerboseLevel)
    {
       printf("DBG: ($filename:%4u): $msg\n", $line);
    }
}


1;
