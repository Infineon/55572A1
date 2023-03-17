#!/usr/bin/perl
#
# Copyright 2016-2023, Cypress Semiconductor Corporation (an Infineon company) or
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

use strict;
use warnings;

use Cwd qw(getcwd);
use FindBin '$Bin';
use lib "$Bin";                    # Directory name of where to look for the rest of the PMs to use.
use File::Path qw(make_path);
use File::Basename;
use File::Copy qw(copy);

use Data::Dumper;

use generaldefines;
use parseCLI;

##########################################################
##                      CONSTANTS                       ##
##                                                      ##
## Command Line Flags:                                  ##
##   Use commas between flags in order to make a list   ##
## of equivalent flags for matching. Used for easy      ##
## shorthand. You can additionally use regex.           ##
##  e.g. HELP => '.*?-h' will match any with -h in it   ##
##########################################################
# Command Line Args
use constant VERBOSE                 => '--verbose=';
use constant HELP                    => '--help';
use constant SIGN                    => '--sign=';
use constant SRC_BTP_FILE            => '--btp=';
use constant SRC_SEC_BIN_FILE        => '--secbin=';
use constant SRC_FW_BIN_FILE         => '--fwbin=';
use constant SRC_APP_BIN_FILE        => '--appbin=';
use constant SRC_SS_BIN_FILE         => '--ssbin=';
use constant SRC_APP_HCD_BIN_FILE    => '--apphcd=';
use constant SRC_FW_HCD_BIN_FILE     => '--fwhcd=';
use constant SRC_HDF_FILE            => '--hdf=';
use constant SRC_SEC_XIP_MDH_FILE    => '--secxipmdh=';
use constant DST_BIN_FILE            => '--subBin=';
use constant DST_HCD_BIN_FILE        => '--subHcdBin=';
use constant DST_AGI_FILE            => '--subAgi=';
use constant DST_HCD_AGI_FILE        => '--subHcdAgi=';
use constant DST_HCD_AGS_FILE        => '--subHcdAgs=';
use constant DST_HCD_SUB_DS          => '--hcd_ds_sub_out=';
use constant DST_CRT_DIR             => '--crt_dir=';
use constant DST_MDH_FILE            => '--mdhBin=';

# Internal constants ... that could be used for command line args one day
use constant DST_ENC_FILE         => '--encBin=';
use constant DST_HCD_ENC_FILE     => '--encHcdBin=';
use constant DST_TBL_FILE         => '--tbl=';
use constant DST_HCD_TBL_FILE     => '--tblHCD=';

##########################################################
##                       GLOBALS                        ##
##                                                      ##
## SETTINGS:                                            ##
##   Use to store all parameters that can change based  ##
## on command line arguments.                           ##
##########################################################
# Command Line Args
my %SETTINGS = (
    &VERBOSE                  => 0,
    &HELP                     => FALSE,
    &SIGN                     => 'n',          # Default to No
    &SRC_SEC_BIN_FILE         => undef,
    &SRC_FW_BIN_FILE          => undef,
    &SRC_BTP_FILE             => undef,
    &SRC_APP_BIN_FILE         => undef,
    &SRC_SS_BIN_FILE          => undef,
    &SRC_APP_HCD_BIN_FILE     => undef,
    &SRC_FW_HCD_BIN_FILE      => undef,
    &SRC_HDF_FILE             => undef,
    &SRC_SEC_XIP_MDH_FILE     => undef,
    &DST_HCD_AGI_FILE         => undef,        # Default to variation of SRC bin file name (in init())
    &DST_HCD_AGS_FILE         => undef,        # Default to variation of SRC bin file name (in init())
    &DST_AGI_FILE             => undef,        # Default to variation of SRC bin file name (in init())
    &DST_BIN_FILE             => undef,        # Default to variation of SRC bin file name (in init())
    &DST_HCD_BIN_FILE         => undef,        # Default to variation of SRC bin file name (in init())
    &DST_HCD_SUB_DS           => undef,
    &DST_CRT_DIR              => undef,
    &DST_ENC_FILE             => undef,
    &DST_TBL_FILE             => undef,
    &DST_MDH_FILE             => undef,
    );
#
# Program variables
#
my $verbose;
my $hcdActive = 0;

my $crt_param = {};
my $hdf_param = {};
my $lst_param = {};
my $btp_param = {};

my @regionBaseKeys = ('SS', 'VS', 'DS');

my @pkgNames = ('SEC', 'FW', 'APP');

my $region_param = {};
$region_param->{"HCD"} = {};

my $BIN_InputHANDLE;
my $AGI_OutputHANDLE;

my $SRC_ALIGN_SIZE = 4;        # Size of file to pad file to
my $SRC_ALIGN_PAD_BYTE = pack ('C',0x00);

my $NO_CERT          = '00';
my $FW_SEC_CERT      = '01';
my $FW_UNSEC_CERT    = '02';
my $CUSTOMER_CERT    = '03';

my $SIGNING_PREFIX_NAME     = undef;
my $SIGNING_PREFIX_NAME_HCD = undef;

# R4 header format
#  8 (BRCMcfgD) +
# 16 (R4 Header of DS,  Checksum & Length) +
#  3 records (3 * 12) Item ID & Group ID & LEB Length & padding +
#     SecSubDS Size (will always exist ... even if its a dummy emptys subds.)
#     DsSubDS Size  the reason why we are here!
#     AppSubDS size (will always exist ... even if its a dummy emptys subds.)
# 12 (end of config record) Item ID & Group ID & LEB Length & padding +
#      Must ALWAYS have the entries of all 3, since headers come before body data whose address is used in signing.
my $R4_HEADER_SIZE = 8 + 16 + (3 * 12) + 12;

exit main();

#-------------------------------------------------------------------------------
# Function      : main
#
# Description   : Main program loop.
#
# Inputs        : Command line args (see above)
#
# Outputs       : Analysis to STDOUT
#
# Returns       : Return code of program (LINUX style)
#                   0  = Success
#                   >0 = Fail
#-------------------------------------------------------------------------------
sub main
{
    my $btp_key;
    my $tmpFilename;
    my $tmpDirs;
    my $tmpSuffix;
    my $dsName;
    my $dst_bin_type = 3;

    # Handle the command line
    if (init() == ERROR_FATAL)
    {
        usage();
        return 1;
    }
    initDBG($verbose);

    printDBG("Createing APP Subds");
    $SIGNING_PREFIX_NAME     = 'app';
    $SIGNING_PREFIX_NAME_HCD = 'app_dl';
    ($tmpFilename, $tmpDirs, $tmpSuffix) = fileparse($SETTINGS{&SRC_APP_BIN_FILE}, qr/\.[^.]*/);
    $dsName = 'APP';

    $SETTINGS{&DST_AGI_FILE}     = $tmpDirs . $tmpFilename . '_sub.agi'      if !defined($SETTINGS{&DST_AGI_FILE});
    $SETTINGS{&DST_BIN_FILE}     = $tmpDirs . $tmpFilename . '_sub_agi.bin'  if !defined($SETTINGS{&DST_BIN_FILE});
    $SETTINGS{&DST_ENC_FILE}     = $tmpDirs . $tmpFilename . '_sub_agi.enc'  if !defined($SETTINGS{&DST_ENC_FILE});
    $SETTINGS{&DST_HCD_AGI_FILE} = $tmpDirs . $tmpFilename . '_sub_hcd.agi'  if !defined($SETTINGS{&DST_HCD_AGI_FILE});
    $SETTINGS{&DST_HCD_AGS_FILE} = $tmpDirs . $tmpFilename . '_sub_hcd.ags'  if !defined($SETTINGS{&DST_HCD_AGS_FILE});
    $SETTINGS{&DST_TBL_FILE}     = $tmpDirs . $tmpFilename . '_sub_agi.tbl' if !defined($SETTINGS{&DST_TBL_FILE});
    $SETTINGS{&DST_HCD_TBL_FILE} = $tmpDirs . $tmpFilename . '_sub_agi_hcd.tbl';

    $region_param->{HCD}{HCD_BASE_NAME} = $tmpDirs . $tmpFilename . '_sub_agi_hcd';

    exit -1 if (parse_btp_file($SETTINGS{&SRC_BTP_FILE}) == ERROR_FATAL);
    exit -1 if (parse_bin_file($SETTINGS{&SRC_SS_BIN_FILE}, $crt_param->{ss_start}, "SS", 0) == ERROR_FATAL);
    exit -1 if (parse_bin_file($SETTINGS{&SRC_SEC_BIN_FILE}, 0, "SEC", $SRC_ALIGN_SIZE) == ERROR_FATAL);
    exit -1 if (parse_bin_file($SETTINGS{&SRC_FW_BIN_FILE}, 0, "FW", $SRC_ALIGN_SIZE) == ERROR_FATAL);
    exit -1 if (parse_bin_file($SETTINGS{&SRC_APP_BIN_FILE}, 0, "APP", $SRC_ALIGN_SIZE) == ERROR_FATAL);

    # only create a subds for the new input ds.
    exit -1 if (create_subDS_bin_file($dsName, $dst_bin_type, $SETTINGS{&DST_BIN_FILE}, $SETTINGS{&DST_TBL_FILE})== ERROR_FATAL);
    exit -1 if (sign_subDS_bin_file($dsName, $SIGNING_PREFIX_NAME, $SETTINGS{&DST_BIN_FILE}, $SETTINGS{&SIGN}) == ERROR_FATAL);

    # HEX creation ... these create file that cgs.exe can use to make a hex output.
    exit -1 if (create_subDS_agi_file($SETTINGS{&DST_AGI_FILE}, $SETTINGS{&SRC_HDF_FILE}) == ERROR_FATAL);

    # HCD creation
    # Since "cgs.exe" can NOT build a DS using R4 and HCD.  We need to take all the HCD-DS files and pack them into a HCD-DS R4 file.
    if ($hcdActive)
    {
        exit -1 if (parseHCD($SETTINGS{&SRC_FW_HCD_BIN_FILE}, "FW", $SRC_ALIGN_SIZE) == ERROR_FATAL);
        exit -1 if (parseHCD($SETTINGS{&SRC_APP_HCD_BIN_FILE}, "APP", $SRC_ALIGN_SIZE) == ERROR_FATAL);

        exit -1 if (create_subDS_HCD_bin_files($dsName, $dst_bin_type, $SETTINGS{&DST_BIN_FILE}) == ERROR_FATAL);
        exit -1 if (sign_subDS_HCD_bin_file($dsName, $SIGNING_PREFIX_NAME_HCD, $SETTINGS{&SIGN}) == ERROR_FATAL);

        exit -1 if (create_full_hcd_file($SETTINGS{&DST_HCD_SUB_DS}) == ERROR_FATAL);
    }

    if (defined($SETTINGS{&SRC_SEC_XIP_MDH_FILE})
         && defined($SETTINGS{&DST_MDH_FILE}))
    {
        exit -1 if (create_mdh_file($SETTINGS{&SRC_SEC_XIP_MDH_FILE}, $SETTINGS{&DST_MDH_FILE})== ERROR_FATAL);
    }

    return 0;
}

#-------------------------------------------------------------------------------
# Function      : parse_SS_bin_file
#
# Description   : Read in the SS bin file and save off the information and data.
#
# Inputs        : ssFile        Name of file
#                 keyName       Name of key to indicate file type in hash
#                 addr          Start address of this file in memory (if known)
#
# Outputs       : N/A
#
# Returns       : Return code of program (LINUX style)
#                   0  = Success
#                   >0 = Fail
#-------------------------------------------------------------------------------
sub parse_bin_file
{
    my ($file, $binAdr, $keyName, $alignSize) = @_;
    my $InputHANDLE;
    my $buffer;

    if (!defined($file))
    {
        printDBG("File not defined");
        return ERROR_FATAL;
    }
    if (!(-e $file))
    {
        printDBG("File missing <$file>");
        return ERROR_FATAL;
    }
    my $binSize = -s $file;

    $region_param->{$keyName}{BIN_NAME} = $file;
    $region_param->{$keyName}{BIN_LEN_RAW} = $binSize;
    $region_param->{$keyName}{BIN_ADR} = $binAdr;

    # if we have an alignment, figure out the padding so we can create empty bytes later.
    $region_param->{$keyName}{BIN_LEN_PAD_SIZE} = 0;
    if ($alignSize)
    {
        my $tmp = $region_param->{$keyName}{BIN_LEN_RAW} % $alignSize;
        if ($tmp)
        {
            $region_param->{$keyName}{BIN_LEN_PAD_SIZE} = $alignSize - $tmp;
        }
    }
    $region_param->{$keyName}{BIN_LEN_PADDED} = $region_param->{$keyName}{BIN_LEN_RAW} + $region_param->{$keyName}{BIN_LEN_PAD_SIZE};

    printDBG("    $keyName file -->            " . $region_param->{$keyName}{BIN_NAME});
    printDBG("    $keyName Bin size is         " . sprintf("0x%08x", $region_param->{$keyName}{BIN_LEN_RAW}),2);
    printDBG("    $keyName Bin pad delta is    " . sprintf("0x%08x", $region_param->{$keyName}{BIN_LEN_PAD_SIZE}),2);
    printDBG("    $keyName Bin align size is   " . sprintf("0x%08x", $region_param->{$keyName}{BIN_LEN_PADDED}),2);
    printDBG("    $keyName Adr is              " . sprintf("0x%08x", $region_param->{$keyName}{BIN_ADR}),2);

    open( $InputHANDLE, "<", $file ) || die ("\nERROR(".__FILE__.":".__LINE__.") : Can't open v\n\n");
    binmode $InputHANDLE, ':raw';

    my $readLen = read ($InputHANDLE, $buffer, $region_param->{$keyName}{BIN_LEN_RAW});
    if ($readLen)
    {
        $region_param->{$keyName}{BIN_DATA_RAW} = $buffer;
    }
    else
    {
        printERR("No data read from $keyName file - $file");
        return ERROR_FATAL;
    }

    # Add any padding...pad them all for now, doesnt matter since our pad size is so small.
    $region_param->{$keyName}{BIN_DATA_PADDED} = padBuf($buffer, $region_param->{$keyName}{BIN_LEN_PAD_SIZE}, $SRC_ALIGN_PAD_BYTE);


    # Figure out the signing address of the block and R4 length
    if ($keyName ne 'SS')
    {
        # add length of SubDS header to length so we can use it later.
        $region_param->{$keyName}{BIN_LEN_PADDED_SUBDS} = $region_param->{$keyName}{BIN_LEN_PADDED} + 12;

        my $signAdr = $crt_param->{ds_start} + $R4_HEADER_SIZE;
        $region_param->{$keyName}{SUB_DS_SIGN_ADR} = $signAdr;

        $signAdr += $region_param->{SEC}{BIN_LEN_PADDED};
        $region_param->{$keyName}{SUB_DS_SIGN_ADR} = $signAdr if ($keyName eq 'FW');

        $signAdr += $region_param->{FW}{BIN_LEN_PADDED} if ($keyName ne 'SEC');
        $region_param->{$keyName}{SUB_DS_SIGN_ADR} = $signAdr if ($keyName eq 'APP');

        # Figure out where the actual DS data starts int the SubDS
        $region_param->{$keyName}{SUB_DS_DATA_VAR_ADR} = $region_param->{$keyName}{SUB_DS_SIGN_ADR} + 12;

        printDBG("    DS-" . $keyName . "  Sign Address is      " . sprintf("0x%08x",$region_param->{$keyName}{SUB_DS_SIGN_ADR}),2);
        printDBG("    DS-" . $keyName . "  data-var Address is  " . sprintf("0x%08x",$region_param->{$keyName}{SUB_DS_DATA_VAR_ADR}),2);
    }
    printBuf($region_param->{$keyName}{BIN_DATA_RAW},    $region_param->{$keyName}{BIN_LEN_RAW},     $region_param->{$keyName}{BIN_NAME}. "  RAW DATA");
    printBuf($region_param->{$keyName}{BIN_DATA_PADDED}, $region_param->{$keyName}{BIN_LEN_PADDED},  $region_param->{$keyName}{BIN_NAME}. "  PADDED DATA");

    close $InputHANDLE;

    # Get Size, Open, Read, Close, Save start address
}


#-------------------------------------------------------------------------------
# Function      : parseHCD
#
# Description   : Read in the HCD file and save the regions in a hash.
#                 The SS is not expected to be in the input file as we are only
#                 dealing with the DS file for signing and encrypting.
#
#                 The DS assumed to be on set of contigous HCD records.
#
#                 All sections are stored in their own hash area.
#
# Inputs        : HCI commands  Below is an example of a DS and 1 write to patch ram.
#                 4c-fc-f4 0x00438000 42 52 43 4d 63 66 67 44 00 00 00 00 fb 00 00 00 ... b0 80 bd
#                 4c-fc-1f 0x004380f0 00 00 03 01 0c b0 69 41 00 79 4c 06 20 01 04 30 ... fe 00 00
#                 4c-fc-50 0x00300400 0e 20 90 71 70 47 80 b5 46 f6 f4 00 40 f2 00 01 ... 47 00 00
#
# Outputs       :
#
# Returns       : Return code of program (LINUX style)
#                   0  = Success
#                   >0 = Fail
#-------------------------------------------------------------------------------
sub parseHCD
{
    my ($hcdFile, $regionKey, $alignSize) = @_;

    my $BIN_InputHANDLE;
    my $srcHcdBinsize = -s $hcdFile;
    my $buffer = ();
    my %hcd = ();
    my @cmdHeader;
    my $cmdAddress;
    my @cmdData;
    my $cmdLen;
    my $hciDataLen;
    my $readLen;
    my $newHcdRegion;

    my $binIndex = -1;  # trick to handle the looping
    my $msg;
    my $dsRef;
    my $hcdRef;

    if (!defined($srcHcdBinsize) || $srcHcdBinsize == 0)
    {
        printERR("Input file <$hcdFile> was empty!");
        return ERROR_FATAL;
    }
    printDBG("Src file <$hcdFile>   HcdBinsize is ". sprintf("0x%08x", $srcHcdBinsize),2);

    open( $BIN_InputHANDLE, "<", $hcdFile ) || die ("\nERROR(".__FILE__.":".__LINE__.") : Can't open $hcdFile\n\n");
    binmode $BIN_InputHANDLE, ':raw';

    $readLen = read ($BIN_InputHANDLE, $buffer, 3);
    while ($readLen)
    {
        @cmdHeader = unpack("C[$readLen]",$buffer);
        my $cmdLen = $cmdHeader[2];
        $hciDataLen = $cmdLen - 4;                # Length of data in the hci record

        $readLen = read ($BIN_InputHANDLE, $buffer, 4);
        $cmdAddress = unpack("L",$buffer);

        printDBG("data Record Length is ". sprintf("%d", $hciDataLen),2);

        if ($hciDataLen == 0)
        {
            $readLen = 0;
            next;
        }
        $readLen = read ($BIN_InputHANDLE, $buffer, $hciDataLen);

        if ($hciDataLen != $readLen)
        {
            printERR ("ERROR: Expected record length doesnt match read length <$hciDataLen - $readLen>\n");
            return ERROR_FATAL;
        }

        if ($cmdAddress == $btp_param->{ConfigDSLocation})
        {
            #  'BRCMcfgD' + 4 byte Checksum + 4 byte Length
            my ($cfgID, $cfgChksum, $cfgLen) = unpack("Z8LL",$buffer);
            my $dsBinCnt = 0;

            printDBG("Found DS record - Config Length is $cfgLen");

            if ($cfgID =~ /BRCMcfgD/)
            {
                $region_param->{HCD}{$regionKey}{DS} = {};
                $dsRef = $region_param->{HCD}{$regionKey}{DS};
                $dsRef->{BIN_DATA_RAW} = $buffer;
                $dsRef->{BIN_ADR} = $cmdAddress;
                $dsRef->{BIN_LEN_RAW} = $cfgLen + 16; # DS raw record + DS header size
                $dsRef->{BIN_LEN_PAD_SIZE} = 0 ;
                if ($alignSize)
                {
                    my $tmp = $dsRef->{BIN_LEN_RAW} % $alignSize;
                    if ($tmp)
                    {
                        $dsRef->{BIN_LEN_PAD_SIZE} = $alignSize - $tmp;
                    }
                }
                $dsRef->{BIN_LEN_PADDED} = $dsRef->{BIN_LEN_RAW} + $dsRef->{BIN_LEN_PAD_SIZE};
                $dsRef->{BIN_NAME} = $region_param->{HCD}{"HCD_BASE_NAME"} . "_ds.bin";
                $dsRef->{ENC_NAME} = $region_param->{HCD}{"HCD_BASE_NAME"} . "_ds.enc";

                # add length of SubDS header to length so we can use it later.
                $dsRef->{BIN_LEN_PADDED_SUBDS} = $dsRef->{BIN_LEN_PADDED} + 12;

                # Save off how much of the DS we already have
                $dsBinCnt = $hciDataLen;

                # Read in the rest of the HCD commands for this config based on how much
                # config data is left.
                #   Subtracting 16 (BRCMcfgD + 4 bytes checksum, 4 bytes length)
                #     this is what we already are putting into the buffer.
                if ($cfgLen >= ($hciDataLen - 16))
                {
                    # Still need to read in this much config data.
                    $cfgLen -= ($hciDataLen - 16);
                    printDBG("DS Start ... still need to read in $cfgLen",3);
                }
                else
                {
                    die "Error doing math\n";
                }

                while ($cfgLen)
                {
                    $readLen = read ($BIN_InputHANDLE, $buffer, 7);
                    my (@cmdHeader) = unpack("C[7]",$buffer);

                    $msg = sprintf("             next address is 0x%02x%02x%02x%02x", $cmdHeader[6],$cmdHeader[5],$cmdHeader[4],$cmdHeader[3]);
                    printDBG($msg,3);

                    $hciDataLen = $cmdHeader[2] - 4;
                    printDBG("             read in $hciDataLen");

                    $readLen = read ($BIN_InputHANDLE, $buffer, $hciDataLen);

                    $dsRef->{BIN_DATA_RAW} .= $buffer;
                    $cfgLen -= $readLen;
                    $dsBinCnt += $readLen;
                    printDBG("             Still need to read in $cfgLen");
                }
                printDBG("             Total DS is $dsBinCnt");
                die "HCD DS Length mismatch " if ($region_param->{HCD}{$regionKey}{DS}{BIN_LEN_RAW} != $dsBinCnt);

                # Add any padding...pad them all for now, doesnt matter since our pad size is so small.
                $dsRef->{BIN_DATA_PADDED} = padBuf($dsRef->{BIN_DATA_RAW}, $dsRef->{BIN_LEN_PAD_SIZE}, $SRC_ALIGN_PAD_BYTE);
                printBuf($dsRef->{BIN_DATA_RAW},    $dsRef->{BIN_LEN_RAW},     $dsRef->{BIN_NAME}. "  RAW DATA");
                printBuf($dsRef->{BIN_DATA_PADDED}, $dsRef->{BIN_LEN_PADDED},  $dsRef->{BIN_NAME}. "  PADDED DATA");
            }
            else
            {
                die "Unknown config record\n";
            }
        }
        else
        {
            # We have a direct load, they are NOT config records.
            #   Record the address, append the data and bump the index number
            #   NEED TO COALESCE CONTIGOUS BLOCKS OF DATA to minimize the number of files to sign
            #
            # See if this HCI record belongs in an existing region or we need to make a new one
            if (($binIndex != -1 && defined($hcdRef))
                &&
                ($hcdRef->{BIN_ADR} + $hcdRef->{BIN_LEN_RAW}) == $cmdAddress)
            {
                my $tmpPrnBuf = sprintf("        %d length is <0x%04x>   address is 0x%08x  NEW-> 0x%08x:0x%04x",$binIndex, $hcdRef->{BIN_LEN_RAW}, $hcdRef->{BIN_ADR}, $cmdAddress,  $hciDataLen);
                printDBG($tmpPrnBuf,3);

                $hcdRef->{BIN_DATA_RAW} .= $buffer;
                $hcdRef->{BIN_LEN_RAW}  += $hciDataLen;
            }
            else
            {
                if (defined($hcdRef))
                {
                    # About to create a new region, if already defined dump it to screen
                    printBuf($hcdRef->{BIN_DATA_RAW},    $hcdRef->{BIN_LEN_RAW},     $hcdRef->{BIN_NAME}. "  RAW DATA");
                    printBuf($hcdRef->{BIN_DATA_PADDED}, $hcdRef->{BIN_LEN_PADDED},  $hcdRef->{BIN_NAME}. "  PADDED DATA");
                }

                $binIndex++;

                printDBG("Section bindex is now $binIndex",3);;

                $region_param->{HCD}{$regionKey}{"SECT_$binIndex"} = {};
                $hcdRef = $region_param->{HCD}{$regionKey}{"SECT_$binIndex"};
                $hcdRef->{BIN_DATA_RAW} = $buffer;
                $hcdRef->{BIN_ADR} = $cmdAddress;
                $hcdRef->{BIN_SIGN_ADR} = $cmdAddress;
                $hcdRef->{BIN_LEN_RAW} = $hciDataLen;

                my $tmp = sprintf("%s_%d", $region_param->{HCD}{"HCD_BASE_NAME"}, $binIndex);
                $hcdRef->{BIN_NAME} =  $tmp . ".bin";
                $hcdRef->{ENC_NAME} =  $tmp . ".enc";

                my $tmpPrnBuf = sprintf("DS_Sec <SECT_%d> length is <%d>   address is 0x%08x",$binIndex, $hciDataLen, $cmdAddress);
                printDBG($tmpPrnBuf,3);
            }

            $hcdRef->{BIN_LEN_PAD_SIZE} = 0;
            if ($alignSize)
            {
                my $tmp = $hcdRef->{BIN_LEN_RAW} % $alignSize;
                if ($tmp)
                {
                    $hcdRef->{BIN_LEN_PAD_SIZE} = $alignSize - $tmp;
                }
            }
            # Add any padding...pad them all for now, doesnt matter since our pad size is so small.
            $hcdRef->{BIN_LEN_PADDED} = $hcdRef->{BIN_LEN_RAW} + $hcdRef->{BIN_LEN_PAD_SIZE};
            $hcdRef->{BIN_DATA_PADDED} = padBuf($hcdRef->{BIN_DATA_RAW}, $hcdRef->{BIN_LEN_PAD_SIZE}, $SRC_ALIGN_PAD_BYTE);

        } # endif

        # Get header of next record
        $readLen = read ($BIN_InputHANDLE, $buffer, 3);
    }#endwhile

    # Print the last hcd region, if we ever had one
    if (defined($hcdRef))
    {
        printBuf($hcdRef->{BIN_DATA_RAW},    $hcdRef->{BIN_LEN_RAW},     $hcdRef->{BIN_NAME}. "  RAW DATA");
        printBuf($hcdRef->{BIN_DATA_PADDED}, $hcdRef->{BIN_LEN_PADDED},  $hcdRef->{BIN_NAME}. "  PADDED DATA");
    }

    # Figure out the signing address of the block
    my $tmpRef = $region_param->{HCD}{$regionKey}{DS};
    my $signAdr = $crt_param->{ds_start} + $R4_HEADER_SIZE;

    #     this is correct as the sec bin is NEVER an hcd file, so use regular bin num
    $signAdr += $region_param->{SEC}{BIN_LEN_PADDED};

    $tmpRef->{SUB_DS_SIGN_ADR} = $signAdr if ($regionKey eq 'FW');

    $signAdr += $region_param->{HCD}{FW}{DS}{BIN_LEN_PADDED} if ($regionKey ne 'SEC');
    $tmpRef->{SUB_DS_SIGN_ADR} = $signAdr                    if ($regionKey eq 'APP');

    # Figure out where the actual DS data starts int the SubDS
    $tmpRef->{SUB_DS_DATA_VAR_ADR} = $tmpRef->{SUB_DS_SIGN_ADR} + 12;

    hcdDebugInfo($regionKey) ;

    close $BIN_InputHANDLE;
}

sub hcdDebugInfo
{
    my ($key) = @_;

    my $hcdDSref = $region_param->{HCD}{$key}{DS};
    printDBG("    HCD $key DS Load Address is      " . sprintf("0x%08x",$hcdDSref->{BIN_ADR}),2);
    printDBG("    HCD $key DS Sign Address is      " . sprintf("0x%08x",$hcdDSref->{SUB_DS_SIGN_ADR}),2);
    printDBG("    HCD $key DS Load Length  is      " . sprintf("0x%08x:%d",$hcdDSref->{BIN_LEN_PADDED}, $hcdDSref->{BIN_LEN_PADDED} ),2);

    my $binIndex = 0;
    my $not_done = 1;
    while ($not_done)
    {
        if (exists( $region_param->{HCD}{$key}{"SECT_$binIndex"}))
        {
            my $hcdDSref = $region_param->{HCD}{$key}{"SECT_$binIndex"};
            printDBG("    HCD $key SECT < $binIndex > Load Address is      " . sprintf("0x%08x",$hcdDSref->{BIN_ADR}),2);
            printDBG("    HCD $key SECT < $binIndex > Sign Address is      " . sprintf("0x%08x",$hcdDSref->{BIN_ADR}),2);
            printDBG("    HCD $key SECT < $binIndex > Load Length  is      " . sprintf("0x%08x:%d",$hcdDSref->{BIN_LEN_PADDED}, $hcdDSref->{BIN_LEN_PADDED} ),2);
            $binIndex++;
        }
        else
        {
            $not_done = 0;
        }
    }
}


#-------------------------------------------------------------------------------
# Function      : create_subDS_bin_file
#
# Description   : Create the sub DS record in a binary file.  This is what is signed.
#                 *** Round up the data size to be 4 byte aligned at the end
#
# Inputs        : Command line args (see above)
#
# Outputs       : DST AGI file
#
# Returns       : Return code of program (LINUX style)
#                   0  = Success
#                   >0 = Fail
#-------------------------------------------------------------------------------
sub create_subDS_bin_file
{
    my ($keyName, $binType, $dstBinFile, $tblFile) = @_;

    my $BIN_InputHANDLE;
    my $BIN_OutputHANDLE;

    my $TBL_OutputHANDLE;
    my ($tmpFilename, $tmpDirs, $tmpSuffix) = fileparse($dstBinFile, qr/\.[^.]*/);
    my $binFileName = $tmpFilename . $tmpSuffix;
    my $tableEntry;

    my $numRead;
    my $data = '';
    my $binline;

    open( $BIN_OutputHANDLE, '>', $dstBinFile) || die ("\nERROR(".__FILE__.":".__LINE__.") : Can't open $dstBinFile\n\n");
    binmode $BIN_OutputHANDLE, ':raw';
    printDBG("Writing File " . $dstBinFile);

    # Print the Length portion

    print $BIN_OutputHANDLE pack("V",$region_param->{$keyName}{BIN_LEN_PADDED});

    # Print the Address portion
    print $BIN_OutputHANDLE pack("V",$region_param->{$keyName}{SUB_DS_DATA_VAR_ADR}); # {sub_ds_data_var_addr});

    # Print the Type portion
    print $BIN_OutputHANDLE pack("V",$binType);

    # Print out the PADDED data, as that what is being signed
    print $BIN_OutputHANDLE $region_param->{$keyName}{BIN_DATA_PADDED};

    close $BIN_OutputHANDLE;

    # Build the signing table
    # Must add 12 to length in table because we just added the SubDS record parameters.
    #     Length and Address must be HEX  ie. 0xNNNNNNNN
    $tableEntry = create_cc312TableEntry($binFileName,
                                         $region_param->{$keyName}{SUB_DS_SIGN_ADR},
                                         $region_param->{$keyName}{BIN_LEN_PADDED_SUBDS} );

    open( $TBL_OutputHANDLE, '>', $tblFile) || die ("\nERROR(".__FILE__.":".__LINE__.") : Can't open $tblFile\n\n");
    print $TBL_OutputHANDLE $tableEntry;
    close $TBL_OutputHANDLE;

    return SUCCESS;
}

#-------------------------------------------------------------------------------
# Function      : sign_subDS_bin_file
#
# Description   : Create the config files for the Certificate processing.
#
# Inputs        :
#
# Outputs       :
#
# Returns       : Return code of program (LINUX style)
#                   0  = Success
#                   >0 = Fail
#-------------------------------------------------------------------------------
sub sign_subDS_bin_file
{
    my ($keyName, $prefix, $srcBinFile, $signFlag) = @_;

    # =================================================
    # Commands for creating certificates on remote Security Cryto server
    #
    my $retVal;
    my $cmd;

    printDBG("   Signing content ... START", 1);

    if ($signFlag =~ /y/)
    {
        my @srcFiles;
        my @dstFiles;

        my $certTable = [
                            "$SETTINGS{&DST_CRT_DIR}/" . $prefix . "_content_cert.bin",
                            "$SETTINGS{&DST_CRT_DIR}/" . $prefix . "_root_key_cert.bin",
                            "$SETTINGS{&DST_CRT_DIR}/" . $prefix . "_key_cert.bin"
                        ];

        push (@srcFiles, $srcBinFile);
        push (@dstFiles, $SETTINGS{&DST_ENC_FILE});

        return $retVal if (($retVal = hsm_sign_tz_file (\@srcFiles,
                                                        $SETTINGS{&DST_TBL_FILE},
                                                        \@dstFiles,
                                                        $prefix,
                                                        $certTable)) != SUCCESS);
    }
    else
    {
        # No signing, just copy the input to the output
        printDBG("        Singing / Encrypting Disabled ... copying input file to output file", 1);
        $cmd = "cp  $srcBinFile $SETTINGS{&DST_ENC_FILE}";
        return $retVal if (($retVal = systemCmd($cmd)) != SUCCESS);
    }

    printDBG("   Signing content ... SUCCESS", 1);

    return SUCCESS;
}

#-------------------------------------------------------------------------------
# Function      : create_subDS_HCD_bin_files
#
# Description   : Create the HCD bin files for the Certificate processing.
#                   Split the HCD file into bin files for each region being written
#                   Create teh table file for the certificate processing
#
# Inputs        :
#
# Outputs       :
#
# Returns       : Return code of program (LINUX style)
#                   0  = Success
#                   >0 = Fail
#-------------------------------------------------------------------------------
sub create_subDS_HCD_bin_files
{
    my ($key, $binType, $dstBinFile) = @_;

    my $TBL_OutputHANDLE;
    my $strSignAddress;
    my $strSignLength;
    my $tmp;
    my $HSL;

    if ((keys %{$region_param->{HCD}{$key}}) == 0)
    {
        # There are no HCD regions
        printINFO("No HCD file found for <$key>\n");
        return SUCCESS;
    }
    $strSignAddress = sprintf("0x%08x", $region_param->{HCD}{$key}{DS}{SUB_DS_SIGN_ADR});

    # Build the signing table
    #   File Name of input to Signing
    #   Start address of where the signed/encrypted blob will be loaded
    #   ?
    #   Size of blob
    #   ?
    # 32Bytes.bin 0x460000 0xffffffff 0x00000020 0x0
    printDBG("HCD tbl file is  $SETTINGS{&DST_HCD_TBL_FILE}",2);
    my $hcdIdx = 0;
    open( $TBL_OutputHANDLE, '>', $SETTINGS{&DST_HCD_TBL_FILE}) || die ("\nERROR(".__FILE__.":".__LINE__.") : Can't open $SETTINGS{&DST_HCD_TBL_FILE}\n\n");

    foreach my $nameKey (sort keys %{$region_param->{HCD}{$key}})
    {
        my $ref = $region_param->{HCD}{$key}{$nameKey};

        my ($tmpFilename, $tmpDirs, $tmpSuffix) = fileparse($ref->{"BIN_NAME"}, qr/\.[^.]*/);
        $tmpFilename = $tmpFilename . $tmpSuffix;

        # Write the HCD data section to specific file for signing
        open( $HSL, '>', $ref->{"BIN_NAME"}) || die ("\nwhoops\n\n");
        binmode $HSL, ':raw';
        printDBG("Writing File " . $ref->{"BIN_NAME"});

        # Print the SUBDS record header ONLY for the DS record ... the address
        # is the same as the regular DS.
        if ($nameKey eq "DS")
        {
            # Print the Length portion
            print $HSL pack("V", $ref->{"BIN_LEN_PADDED"});

            # Print the Address portion
            print $HSL pack("V",$ref->{"SUB_DS_DATA_VAR_ADR"});

            # Print the Type portion
            print $HSL pack("V",$binType);
        }
        print $HSL $ref->{"BIN_DATA_PADDED"};

        close $HSL;

        #####################################################################
        # TABLE ENTRY
        #     The signing address is the same for unsecure subds no matter if HCD or not.
        if ($nameKey =~ "DS")
        {
            # Must add 12 to length in table because we just added the SubDS record parameters.
            #     Length and Address must be HEX  ie. 0xNNNNNNNN
            $tmp = create_cc312TableEntry($tmpFilename, $ref->{"SUB_DS_SIGN_ADR"}, $ref->{"BIN_LEN_PADDED_SUBDS"});

        }
        elsif ($nameKey =~ /SECT_/)
        {
            # Length and Address must be HEX  ie. 0xNNNNNNNN
            $tmp = create_cc312TableEntry($tmpFilename, $ref->{"BIN_SIGN_ADR"}, $ref->{"BIN_LEN_PADDED"});
        }
        else
        {
            printERR("Unknown region name <$nameKey>\n");
            return ERROR_FATAL;
        }
        print $TBL_OutputHANDLE $tmp;


        $hcdIdx += 1;
    }
    close $TBL_OutputHANDLE;

    return SUCCESS;
}

#-------------------------------------------------------------------------------
# Function      : sign_subDS_HCD_bin_file
#
# Description   : Create the HCD bin files for the Certificate processing.
#                   Split the HCD file into bin files for each region being written
#                   Create teh table file for the certificate processing
#
# Inputs        :
#
# Outputs       :
#
# Returns       : Return code of program (LINUX style)
#                   0  = Success
#                   >0 = Fail
#-------------------------------------------------------------------------------
sub sign_subDS_HCD_bin_file
{
    my ($keyPrefix, $hsmCertPrefix, $signFlag) = @_;

    if ((keys %{$region_param->{HCD}{$keyPrefix}}) == 0)
    {
        # There are no HCD regions
        printINFO("No HCD file found\n");
        return SUCCESS;
    }

    print ("\n   Signing HCD content ... START\n");

    if ($signFlag =~ /y/)
    {
        my $certTable = [
                            "$SETTINGS{&DST_CRT_DIR}/" . $hsmCertPrefix . "_content_cert.bin",
                            "$SETTINGS{&DST_CRT_DIR}/" . $hsmCertPrefix . "_root_key_cert.bin",
                            "$SETTINGS{&DST_CRT_DIR}/" . $hsmCertPrefix . "_key_cert.bin"
                        ];

        my @srcTable;
        my @dstTable;
        my $retVal;

        foreach my $nameKey (keys %{$region_param->{HCD}{$keyPrefix}})
        {
            my $ref = $region_param->{HCD}{$keyPrefix}{$nameKey};
            push(@srcTable, $ref->{"BIN_NAME"});
            push(@dstTable, $ref->{"ENC_NAME"});
        }

        return $retVal if (($retVal = hsm_sign_tz_file (\@srcTable,
                                                        $SETTINGS{&DST_HCD_TBL_FILE},
                                                        \@dstTable,
                                                        $hsmCertPrefix,
                                                        $certTable)) != SUCCESS);

    }
    else
    {
        my $cmd;
        my $retVal;

        # No signing, just copy the input to the output
        print "        Singing / Encrypting Disabled ... copying input files to output files\n";
        foreach my $nameKey (keys %{$region_param->{HCD}{$keyPrefix}})
        {
            my $ref = $region_param->{HCD}{$keyPrefix}{$nameKey};
            $cmd = "cp  $ref->{BIN_NAME} $ref->{ENC_NAME}";
            return $retVal if (($retVal = systemCmd($cmd)) != SUCCESS);
        }
    }
    print ("   Signing HCD content ... SUCCESS\n");

    return SUCCESS;

}
#-------------------------------------------------------------------------------
# Function      : create_cc312TableEntry
#
# Description   : Convert numeric values to a string for the CC312 crypto table.
#                  File Name of input to Signing
#                  Start address of where the signed/encrypted blob will be loaded
#                  Value N/A
#                  Size of blob
#                  Value N/A
#
#                  32Bytes.bin 0x460000 0xffffffff 0x00000020 0x0
#
# Inputs        : filename
#                  address
#                  length
#
# Outputs       : N/A
#
# Returns       : string representing table entry
#-------------------------------------------------------------------------------
sub create_cc312TableEntry
{
    my ($fileName, $address, $length) = @_;

    my $tmp;

    # Length and Address must be HEX  ie. 0xNNNNNNNN
    $tmp = sprintf("%s 0x%08x 0xffffffff 0x%08x 0x0\n", $fileName,  $address, $length);
    printDBG ("Table Entry ->  $tmp",2);

    return $tmp;

}
#-------------------------------------------------------------------------------
# Function      : create_subDS_agi_file
#
# Description   : Populate the AGI template file and output to the new DST file.
#
# Inputs        : Command line args (see above)
#
# Outputs       : DST AGI file
#
# Returns       : Return code of program (LINUX style)
#                   0  = Success
#                   >0 = Fail
#-------------------------------------------------------------------------------
sub create_subDS_agi_file
{
    my ($dstAgiFile, $hdfFile) = @_;

    my $BIN_InputHANDLE;
    my $AGI_OutputHANDLE;

    my $keyType;
    my @lines;
    my @bytes;
    my $count = 0;
    my $dataHeaderFound = 0;
    my $numRead;
    my $data = '';
    my $binline;

    my $fileName;

    open($AGI_OutputHANDLE, '>', $dstAgiFile) || die("\nERROR(".__FILE__.":".__LINE__.") : Can't open $dstAgiFile\n\n");

    print $AGI_OutputHANDLE "# DO NOT EDIT MANUALLY! FW2 VERSION\n";
    print $AGI_OutputHANDLE 'DEFINITION <' . fileparse($hdfFile) . ">\n";

    $data = '';

    foreach my $keyType (@pkgNames)
    {
        $data = '';

        if ($keyType eq 'SEC')
        {
            $fileName = $SETTINGS{&SRC_SEC_BIN_FILE};
        }
        elsif ($keyType eq 'FW')
        {
            $fileName = $SETTINGS{&SRC_FW_BIN_FILE};
        }
        elsif ($keyType eq 'APP')
        {
            $fileName = $SETTINGS{&DST_ENC_FILE};
        }

        if (!defined($fileName) || !(-e $fileName))
        {
            die "Missing $keyType DS file  $fileName\n";
        }
        open( $BIN_InputHANDLE, "<", $fileName ) || die ("\nERROR(".__FILE__.":".__LINE__.") : Can't open $fileName\n\n");
        binmode $BIN_InputHANDLE, ':raw';

        print $AGI_OutputHANDLE "\n";
        print $AGI_OutputHANDLE "ENTRY \"SubDS\" = \"$keyType SUB_DS from $fileName\"\n";
        print $AGI_OutputHANDLE "{\n";
        print $AGI_OutputHANDLE "    \"Data\" =\n";
        print $AGI_OutputHANDLE "    COMMENTED_BYTES\n";
        print $AGI_OutputHANDLE "    {\n";
        print $AGI_OutputHANDLE "        <hex>\n";

        $numRead = read ($BIN_InputHANDLE, $data, 96);

        while ($numRead)
        {
            @bytes = ();
            $count = 0;

            @bytes = unpack "C*", $data;

            foreach my $byte (@bytes)
            {
                $binline .= sprintf "%02x ", $byte;
                $count++;
                if($count == 16)
                {
                    print $AGI_OutputHANDLE "        $binline\n";
                    $count = 0;
                    $binline = "";
                }
            }
            if ($count)
            {
                print $AGI_OutputHANDLE "        $binline\n";
                $count = 0;
                $binline = "";
            }

            $data = '';
            $numRead = read ($BIN_InputHANDLE, $data, 96);
        }
        print $AGI_OutputHANDLE "    } END_COMMENTED_BYTES\n";
        print $AGI_OutputHANDLE "}\n";

        close $BIN_InputHANDLE;
    }

    close $AGI_OutputHANDLE;

    return SUCCESS;
}

#-------------------------------------------------------------------------------
# Function      : create_full_hcd_file
#
# Description   : This routine uses the entries in the HCD table file as well
#                 as other files to build a COMPLETE SS and DS with the SEC, UNSEC and
#                 APP (dummy) SubDS records and any outside (ie.pram sections)
#                 in HCD format with an R4 header.
#
#                  R4 Format
#                    Field Name      Value        Size              Notes
#                                                (bytes)
#                    Header          BRCMcfgD      8
#                    Checksum        --            4                Not used
#                    Length          --            4                Length of data after this field to end of file
#                    Header Table
#                      Item ID       2D            1                Confid Item ID
#                      Group ID      01            1
#                      Length        --            ?                LEB format (look at FW code)
#                      Padding       --            ?                    Depends on number of bytes above to == 4 total
#
#                    Sub DS 1
#                      Item ID       34            1                Confid Item ID
#                      Group ID      01            1
#                      Length        --            4                Not LEB
#                      Offset        --            4                Number of bytes past the header where the data
#                                                                   starts for this record.
#                    Sub DS N
#                      Item ID       34            1                Confid Item ID
#                      Group ID      01            1
#                      Length        --            4                Not LEB
#                      Offset        --            4                Number of bytes past the header where the data
#                                                                        starts for this record.
#                    TERMINATOR
#                      Item ID       FE            1                Confid Item ID
#                      Group ID      00            1
#                      Length        --            4                Not LEB
#                      Offset        --            4                Number of bytes past the header where the data
#                                                                        starts for this record.
#                    Data
#
#                Record Format        Command types        Length        Address            Data
#                -----------------------------------------------------------------------------------
#                Body record          4cfc                 nn            nn nn nn nn        a ... b
#                Terminator record    4efc                 04            ff ff ff ff        --
#
# Inputs        : Command line args (see above)
#
# Outputs       : DST AGI file
#
# Returns       : Return code of program (LINUX style)
#                   0  = Success
#                   >0 = Fail
#-------------------------------------------------------------------------------
sub create_full_hcd_file
{
    my ($outFileHcd) = @_;

    my $HCD_OutputHANDLE;
    my $BIN_InputHANDLE;
    my $BIN_OutputHANDLE;
    my $data;
    my $R4offset = 0;
    my $numRead;
    my $totNumRead;
    my $dsLength;
    my $hcdRec;
    my $address;
    my $NUMRECS = 3;        # we always create 3 records in the header table
                            # Secure - Unsecure - Application
    my $dbgMsg;

    # open hcd file
    open( $HCD_OutputHANDLE, '>', $outFileHcd) || die ("\nERROR(".__FILE__.":".__LINE__.") : Can't open $outFileHcd\n\n");
    binmode $HCD_OutputHANDLE, ':raw';

    # Make HCD for SS and put in file/blob/hash
    #    Convert to HCD format
    printDBG("  Writing SS");
    write_HCD_data_record($HCD_OutputHANDLE, $region_param->{SS}{BIN_LEN_RAW}, $region_param->{SS}{BIN_ADR}, $region_param->{SS}{BIN_DATA_RAW});

    #########################################################
    # Make R4 Header for DS and write to Binary or a Data blob or Data Hash
    #    Convert to HCD format
    #########################################################
    printDBG( "   HCD writing DS");
    $address = $crt_param->{ds_start};
    $hcdRec = "BRCMcfgD";    # Header string
    $hcdRec .= pack("V", 0); # Checksum

    #  8 (Number of entries record)Item ID & Group ID
    #  3 records (3 * 12) Item ID & Group ID & LEB Length & padding +
    # 12 (end of config record) Item ID & Group ID & LEB Length & padding +
    # size_of_SecureDS + size_of_unsecure_ds +
    # size_of_app_ds
    $dsLength = 8 + ($NUMRECS * 12)  + 12;
    $dsLength +=  $region_param->{SEC}{BIN_LEN_PADDED} ;
    $dsLength +=  $region_param->{HCD}{FW}{DS}{BIN_LEN_PADDED_SUBDS};
    $dsLength +=  $region_param->{HCD}{APP}{DS}{BIN_LEN_PADDED_SUBDS};
    $hcdRec .= pack("V", $dsLength);  # Length

    # Create Table Record, add 1 for terminator record
    create_R4_Header_Table_Record(0x2D, 0x01, $NUMRECS+1, \$hcdRec);

    # Create the Header table entry Records
    $R4offset = 0;
    create_R4_Header_Entry_Record(0x34, 0x01, $region_param->{SEC}{BIN_LEN_PADDED}, $R4offset, 0, \$hcdRec);  # Secure
    $R4offset += $region_param->{SEC}{BIN_LEN_PADDED};

    create_R4_Header_Entry_Record(0x34, 0x01, $region_param->{HCD}{FW}{DS}{BIN_LEN_PADDED_SUBDS}, $R4offset, 0, \$hcdRec);  # Firmware
    $R4offset += $region_param->{HCD}{FW}{DS}{BIN_LEN_PADDED_SUBDS};

    create_R4_Header_Entry_Record(0x34, 0x01, $region_param->{HCD}{APP}{DS}{BIN_LEN_PADDED_SUBDS}, $R4offset, 0, \$hcdRec);  # App (real)
    $R4offset += $region_param->{HCD}{APP}{DS}{BIN_LEN_PADDED_SUBDS};

    create_R4_Header_Entry_Record(0xFE, 0x00, 0, $R4offset, 0, \$hcdRec);  # Terminator

    my $lllen = length $hcdRec;
    write_HCD_data_record($HCD_OutputHANDLE, $lllen, $address, $hcdRec);
    $address += $R4_HEADER_SIZE;

    #########################################################
    # Add DS binary data to blobs/hash by address base
    #    Convert to HCD format
    ##################################################
    foreach my $pkg (@pkgNames)
    {
        printDBG(  "   HCD writing $pkg SIGNED data");
        if ($pkg =~ /SEC/)
        {
            my $ref = $region_param->{SEC};
            write_HCD_data_block($HCD_OutputHANDLE,
                                 $ref->{BIN_LEN_PADDED},
                                 $address,
                                 $ref->{BIN_DATA_PADDED});
            $address += $ref->{BIN_LEN_PADDED};
        }
        else
        {
            my $ref = $region_param->{HCD}{$pkg}{DS};
            my $fileSize = -s $ref->{ENC_NAME};
            printDBG("   HCD writing Unsecure SIGNED data");
            printDBG("     From:  $ref->{ENC_NAME}");
            open( $BIN_InputHANDLE, "<", $ref->{ENC_NAME} ) || die ("\nERROR(".__FILE__.":".__LINE__.") : Can't open $ref->{ENC_NAME}\n\n");
            binmode $BIN_InputHANDLE, ':raw';
            $numRead = read ($BIN_InputHANDLE, $data, $fileSize);
            close $BIN_InputHANDLE;
            write_HCD_data_block($HCD_OutputHANDLE,
                                 $fileSize,
                                 $address,
                                 $data);
            $address += $fileSize;
        }

        if ($pkg !~ /SEC/)
        {
            #now you have to get all the random $region_param->{HCD}{SECT_n} regions and convert them
            #to normal HCD commands...easy peasy!
            foreach my $nameKey (keys %{$region_param->{HCD}{$pkg}})
            {
                next if ($nameKey !~ /SECT_/);

                my $ref = $region_param->{HCD}{$pkg}{$nameKey};
                my $fileSize;
                my $addr;

                # Only read in files for the block being parsed
                if ($pkg =~ /APP/)
                {
                    $fileSize = -s $ref->{ENC_NAME};
                    $addr = $ref->{BIN_SIGN_ADR};
                    printDBG("   HCD writing Section $nameKey SIGNED datan", 2);
                    printDBG("     From:  $ref->{ENC_NAME}", 2);
                    open( $BIN_InputHANDLE, "<", $ref->{ENC_NAME} ) || die ("\nERROR(".__FILE__.":".__LINE__.") : Can't open $ref->{ENC_NAME}\n\n");
                    binmode $BIN_InputHANDLE, ':raw';
                    $numRead = read ($BIN_InputHANDLE, $data, $fileSize);
                    close $BIN_InputHANDLE;
                }
                else
                {
                    # Dont manipulate this data, just write out what was originally read in.
                    $data = $ref->{BIN_DATA_PADDED};
                    $fileSize = $ref->{BIN_LEN_PADDED};
                    $addr = $ref->{BIN_SIGN_ADR};
                }
                write_HCD_data_block($HCD_OutputHANDLE,
                                     $fileSize,
                                     $addr,
                                     $data);
                $address += $fileSize;
            }
        }
    }

    write_HCD_terminator_record($HCD_OutputHANDLE);

    close $HCD_OutputHANDLE;

    return SUCCESS;
}

sub write_HCD_data_block
{
    my ($HCD_OutputHANDLE, $recLen, $address, $data) = @_;

    my @dataBody = unpack("C*",$data);
    my $tmpLen = 0;
    my $RECORD_LENGTH_MAX = 208;  #Power of 16.

    $address = $address;
    while ($recLen)
    {
        if ($recLen > $RECORD_LENGTH_MAX)
        {
            $tmpLen = $RECORD_LENGTH_MAX;
        }
        else
        {
            $tmpLen = $recLen;
        }
        my @recArray = splice(@dataBody, 0, $tmpLen);
        my $rec = pack("C*",@recArray);

        write_HCD_data_record($HCD_OutputHANDLE, $tmpLen, $address, $rec);
        $address += $tmpLen;
        $recLen  -= $tmpLen;
    }

}
sub create_R4_Header_Table_Record
{
    my ($itemID, $groupID, $numEntries, $refData) = @_;

    # ASSUME starting at 32 bit boundary
    $$refData .= pack("C", $itemID);
    $$refData .= pack("C", $groupID);
    $$refData .= pack("C", 0x00);
    $$refData .= pack("C", 0x00);
    $$refData .= pack("V", $numEntries);
}

sub create_R4_Header_Entry_Record
{
    my ($itemID, $groupID, $length, $offset, $lebFlag, $refData) = @_;

    my $padding = 0;
    my $recLength = $length;  # total length of data record

    # ASSUME starting at 32 bit boundary
    $$refData .=  pack("C", $itemID);
    $$refData .=  pack("C", $groupID);
    $recLength += 2;

    if ($lebFlag)
    {
        # this is not fully complete..it just handles the length less
        # than 128 and assumes the length starts in the 3rd byte.
        $$refData .=  pack("C", $length);
        $padding = 1 if ($length < 128);

        while ($padding > 0)
        {
            $$refData .= pack("C", 0x00);
            $padding--;
            $recLength += 1;
        }
    }
    else
    {
        $$refData .= pack("C", 0x00);
        $$refData .= pack("C", 0x00);
        $$refData .= pack("V", $length);
        $recLength += (2 + 4);
    }

    if (defined($offset))
    {
        $$refData .= pack("V", $offset);
        $recLength += 4;
    }
}

#  4c-fc-f4 0x00438000 42 52 43 4d 63 66 67 44 00 00 00 00 fb 00 00 00 ... b0 80 bd
sub write_HCD_data_record
{
    my ($fileHandle, $length, $address, $data) = @_;

    if ($length > 251 ||$length < 0 )
    {
        die "length is too big or too small   $length\n";
    }


    # add the address length
    $length += 4;

    if ($verbose > 1)
    {
        my $idx = 0;
        my @cmdBody = unpack("C*",$data);
        printf ("4c fc %02x %08x ",$length, $address);

        for(@cmdBody)
        {
             printf("%02x ", $_ );
             if ($idx >= 15)
             {
                 print"\n";
                 print "                  ";
                 $idx = 0;
             }
             else
             {
                 $idx++;
             }
        }
        print"\n\n";

    }
    print $fileHandle pack("C", 0x4c);
    print $fileHandle pack("C", 0xfc);
    print $fileHandle pack("C", $length);
    print $fileHandle pack("V", $address);
    print $fileHandle $data;
}
sub write_HCD_terminator_record
{
    my ($fileHandle) = @_;

    print $fileHandle pack("C", 0x4e);
    print $fileHandle pack("C", 0xfc);
    print $fileHandle pack("C", 0x04);
    print $fileHandle pack("V", 0xFFFFFFFF);
    printf ("4e fc 04 FFFFFFFF\n")if ($verbose > 1);
}

sub parse_btp_file
{
    my $ds_start;
    my $ss_start;
    my $btp_key;

    # Read in the list file ... populate a hash
    open(my $BTP, "<", $SETTINGS{&SRC_BTP_FILE}) || die "Could not open *.btp file \"$SETTINGS{&SRC_BTP_FILE}\", $!";
    while(defined(my $line = <$BTP>))
    {
        if($line =~ /(\w+)\s*\=\s*(0x[0-9A-Fa-f]+)/)
        {
            $btp_param->{$1} = hex($2);
        }
        elsif($line =~ /(\w+)\s*\=\s*(.*)$/)
        {
            $btp_param->{$1} = $2;
        }
    }
    close $BTP;

    # This is ANNOYING ... they keys can be spelled differently, prefix and suffix differently
    #    so need to loop thru all keys to find them
    foreach $btp_key (keys %$btp_param)
    {
        $ds_start = $btp_param->{$btp_key} if ($btp_key =~ /ConfigDSLocation/);
        $ss_start = $btp_param->{$btp_key} if ($btp_key =~ /ConfigSSLocation/);
    }

    if (!defined $ds_start)
    {
        printERR("Could not locate ConfigDSLocation entry in btp file \"$SETTINGS{&SRC_BTP_FILE}\"\n");
        return ERROR_FATAL;
    }

    if (!defined $ss_start)
    {
        printERR("Could not locate ConfigDSLocation entry in btp file \"$SETTINGS{&SRC_BTP_FILE}\"\n");
        return ERROR_FATAL;
    }

    $crt_param->{ds_start} = $ds_start;
    $crt_param->{ss_start} = $ss_start;

    printDBG("    BTP SS loadAddress is ". sprintf("0x%08x", $crt_param->{ss_start}),2);
    printDBG("    BTP DS loadAddress is ". sprintf("0x%08x", $crt_param->{ds_start}),2);

    return SUCCESS;
}

sub encode_127
{
    my ($val) = @_;
    my $str;
    do {
        my $byte = $val & 0x7f;
        $val >>= 7;
        $byte |= 0x80 if($val > 0);
        $str .= pack "C", $byte;
    } while($val);
    return $str;
}

sub create_mdh_file
{
    my ($sec_xip_mdh_file, $dst_mdh_file) = @_;

    # Read secure XIP MDH
    open(my $sec_xip_mdh_handle, "<", $sec_xip_mdh_file) || die ("\nERROR(".__FILE__.":".__LINE__.") : Can't open $sec_xip_mdh_file\n");
    binmode $sec_xip_mdh_handle, ':raw';

    my $read_len = read($sec_xip_mdh_handle, my $sec_xip_mdh_data, 12);
    die "Invalid MDH file $sec_xip_mdh_file\n" unless $read_len == 12;

    close $sec_xip_mdh_handle;

    # Generate the final MDH file
    my $xip_offset = 0x00100000;

    open(my $dst_mdh_handle, '>', $dst_mdh_file) || die ("\nERROR(".__FILE__.":".__LINE__.") : Can't open $dst_mdh_file\n");
    binmode $dst_mdh_handle, ':raw';

    # Secure XIP
    print $dst_mdh_handle $sec_xip_mdh_data;

    # Secure FW
    print $dst_mdh_handle pack("LLSS",
        $xip_offset + $region_param->{SEC}{SUB_DS_SIGN_ADR}, $region_param->{SEC}{BIN_LEN_PADDED}, 1, 1);

    # FW
    print $dst_mdh_handle pack("LLSS",
        $xip_offset + $region_param->{FW}{SUB_DS_SIGN_ADR}, $region_param->{FW}{BIN_LEN_PADDED}, 0, 0);

    # App
    print $dst_mdh_handle pack("LLSS",
        $xip_offset + $region_param->{APP}{SUB_DS_SIGN_ADR}, $region_param->{APP}{BIN_LEN_PADDED_SUBDS}, 0, 0);

    # Terminator
    print $dst_mdh_handle pack("LLSS", 0, 0, 0, 0);

    close $dst_mdh_handle;

    return SUCCESS;
}

################################################################################
################################################################################
################################################################################
################################################################################
#-------------------------------------------------------------------------------
# Function      : systemCmd
#
# Description   : Simple helper routine to issue System commands
#
# Inputs        : Command to be executed in the shell
#
# Outputs       : Printing of ERROR message to the user on fail if verbose is active
#
# Returns       : Error code on error or SUCCESS
#-------------------------------------------------------------------------------
sub systemCmd
{
    my ($cmd) = @_;

    my $retVal;

    print "$cmd\n" if ($verbose > 1);
    $retVal = system($cmd);
    if ($retVal != 0)
    {
        printERR("\nFAILED Command -->  $cmd\n\n");
        return ERROR_FATAL;
    }
    return SUCCESS;
}

sub padBuf
{
    my ($data, $len, $padVal) = @_;

    die "Empty buffer sent to padBuf" if (!defined($data));
    die "Undefined length sent to padBuf" if (!defined($len));

    # Add any padding...pad them all for now, doesnt matter since our pad size is so small.
    while ($len)
    {
        $data .= $padVal;
        $len--;
    }
    return $data;
}

sub printBuf
{
    my ($data, $len, $title) = @_;

    my $address = 0;

    if ($verbose > 3)
    {
        my @cmdBody = unpack("C[$len]",$data);
        my $idx = 0;

        print "    $title\n";
        printf("          0x%08x: <", $idx);
        for(@cmdBody)
        {
             printf("%02x ", $_ );
             $idx++;
             $address++;
             if ($idx > 15)
             {
                 print"\n";
                 printf("          0x%08x:  ", $address);
                 $idx = 0;
             }
        }
        print ">\n";
    }
}

################################################################################
################################################################################
################################################################################
################################################################################

#-------------------------------------------------------------------------------
# Function      : init
#
# Description   : Parses ARGV for any command arguments. Additionally ensure a
#                 proper setup was given here instead of in main. On setup
#                 error return ERROR_FATAL
#
# Inputs        : @ARGV
#
# Outputs       : SETTINGS hash is configured with command line values.
#
# Returns       : SUCCESS, ERROR_FATAL
#-------------------------------------------------------------------------------
sub init
{

    # Parse command line arguments ignoring errors caused by unmatched flags (these
    #       are probably makefile flags)

    #print Dumper (@ARGV);
    if (SUCCESS != parseCLI::parseCommandLineArguments(\%SETTINGS, \@ARGV, FALSE))
    {
        parseCLI::printErrors();

        return ERROR_FATAL;
    }

    # This check is to allow user to ask for help!
    if ($SETTINGS{&HELP})
    {
        return ERROR_FATAL;
    }

    if (defined($SETTINGS{&VERBOSE}))
    {
        $verbose = $SETTINGS{&VERBOSE};
    }

    if ( !defined($SETTINGS{&SRC_BTP_FILE}) ||
         !defined($SETTINGS{&SRC_HDF_FILE})
       )
    {
        return ERROR_FATAL;
    }
    return ERROR_FATAL if (checkArgFile(&SRC_BTP_FILE) != SUCCESS);
    return ERROR_FATAL if (checkArgFile(&SRC_HDF_FILE) != SUCCESS);
    return ERROR_FATAL if (checkArgFile(&SRC_SEC_BIN_FILE) != SUCCESS);
    return ERROR_FATAL if (checkArgFile(&SRC_FW_BIN_FILE) != SUCCESS);
    return ERROR_FATAL if (checkArgFile(&SRC_APP_BIN_FILE) != SUCCESS);

    if (defined($SETTINGS{&DST_HCD_SUB_DS})
        && defined($SETTINGS{&SRC_FW_HCD_BIN_FILE})
        && defined($SETTINGS{&SRC_APP_HCD_BIN_FILE}))
    {
        $hcdActive = 1;
    }

    return SUCCESS;
}

sub checkArgFile
{
    my ($argName) = @_;

    my $file = $SETTINGS{$argName};

    # Does the location exist, it could be a directory or a file.
    if (-e $file)
    {
        # It must be a file!
        if (-f $file)
        {
            # Must be something in the file
            my $binLen = -s $file;
            if (!defined($binLen) || $binLen == 0)
            {
                printERR(" Invalid  $argName    file is empty <$file>\n");
                return ERROR_FATAL;
            }
        }
        else
        {
            printERR(" Invalid  $argName    file is NOT a file <$file>\n");
            return ERROR_FATAL;
        }
    }
    else
    {
        printERR(" Invalid  $argName    file does not exist <$file>\n");
        return ERROR_FATAL;
    }
    return SUCCESS;
}
#-------------------------------------------------------------------------------
# Function      : usage
#
# Description   : Displays usage information for this script.  The description contains
#                 Keywords that control operation of the command line.  If the command line
#                 does not match the keyword used, the error message is displayed.
#
#                 Keyword:
#                    REQUIRED    Parameter MUST be defined on the command line with a value.
#                    OPTIONAL    Parameter is not required on the command line.
#
# Inputs        : Nothing is passed in directly, this routine calls another to actually parse both
#                 the command line and the rules implented in the usage variable.
#
# Outputs       : Printing of help information if the conditions fail in the usage description.
#
# Returns       : N/A
#-------------------------------------------------------------------------------
sub usage
{
    # Splits allow only the first setting to be shown. This should be the most verbose
    # setting tag
    my @usage = (
        "\n",
        "Usage: perl ".__FILE__." <SETTINGS>\n",
        "  This script creates elf symbol output files.\n\n",
        "  Options: (Spaces matter!)\n\n",
        "    Parameter => Description\n",
        "      ".(split(/, */,HELP))[0]." => Displays this usage message.\n",
        "      ".(split(/, */,SRC_BTP_FILE))[0]              ." => MANDATORY  Input btp file to get the load SS and DS addresses\n",
        "      ".(split(/, */,SRC_SEC_BIN_FILE))[0]          ." => MANDATORY  Input Secure SubDS-BIN subds file to be placed into AGI file\n",
        "      ".(split(/, */,SRC_FW_BIN_FILE))[0]           ." => MANDATORY  Input Firmware SubDS-BIN subds file to be placed into AGI file\n",
        "      ".(split(/, */,SRC_APP_BIN_FILE))[0]          ." => OPTIONAL  Input App DS SubDS-BIN file to be placed into AGI file\n",
        "      ".(split(/, */,SRC_SS_BIN_FILE))[0]           ." => MANDATORY  Input SS binary file used for creating output files\n",
        "      ".(split(/, */,SRC_HDF_FILE))[0]              ." => MANDATORY  Input HDF file name to replace name in the agi file with correct version name\n",
        "      ".(split(/, */,SRC_APP_HCD_BIN_FILE))[0]      ." => OPTIONAL  Input hcd format file\n",
        "      ".(split(/, */,SRC_FW_HCD_BIN_FILE))[0]       ." => OPTIONAL  Input hcd format file\n",
        "      ".(split(/, */,DST_CRT_DIR))[0]               ." => MANDATORY  Location to place calculated certificates\n",
        "      ".(split(/, */,DST_AGI_FILE))[0]              ." => OPTIONAL  Output HCD AGI text file\n",
        "      ".(split(/, */,DST_HCD_AGI_FILE))[0]          ." => OPTIONAL  Output AGI text file\n",
        "      ".(split(/, */,DST_HCD_AGS_FILE))[0]          ." => OPTIONAL  Output AGS text file\n",
        "      ".(split(/, */,DST_BIN_FILE))[0]              ." => OPTIONAL  Clear utput AGI binary file\n",
        "      ".(split(/, */,DST_ENC_FILE))[0]              ." => OPTIONAL  Signed/encrypted output AGI binary file\n",
        "      ".(split(/, */,VERBOSE))[0]                   ." => OPTIONAL  Controls detailed printing (0 = off, 1 = on, default=0).\n",
        "\n",
        "  Notes:\n",
        "    This script CAN be called manually.\n",
        "    For more information read the notes at the top of this script",
        "\n"
    );

    print (parseCLI::lineupBasedOn('=>', 120, @usage));
}


__END__
