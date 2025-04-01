#!/usr/bin/perl
#
# Copyright 2016-2025, Cypress Semiconductor Corporation (an Infineon company) or
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

use warnings;
use strict;

main();

sub main
{
    my @ds_lines;
    my @dl_lines;
    my $bracket_count=0;
    my $cgs_file = $ARGV[0];
    my $direct_load_lines = 0;
    my $got_hdf = 0;

    die "Usage $0 <cgs file>\n" if !defined $cgs_file;
    die "Could not find file \"$cgs_file\"\n" unless -e $cgs_file;

    open(my $CGS_IN, "<", $cgs_file) || die "ERROR: Cannot open $cgs_file, $!";
    while(defined(my $line = <$CGS_IN>)) {
        if(!$got_hdf) {
            push(@dl_lines, $line);
            push(@ds_lines, $line);
            if($line =~ /^\s*DEFINITION\s*\</) {
                $got_hdf = 1;
            }
            next;
        }
        $bracket_count++ if $line =~ /^\s*\{/;
        $bracket_count-- if $line =~ /^\s*\}/;
        if($bracket_count == 0) {
            if($line =~ /^\s*DIRECT_LOAD\s*=/) {
                if($line !~ /\.pre_init_cfg /) {
                    $direct_load_lines = 1;
                }
            }
            if($line =~ /^\s*ENTRY\s*=/) {
                $direct_load_lines = 0;
            }
        }
        if($direct_load_lines)
        {
            push(@dl_lines, $line);
        }
        else {
            push(@ds_lines, $line);
        }
    }
    close $CGS_IN;

    my $cgs_dl_file = $cgs_file;
    $cgs_dl_file =~ s/\.cgs$/_direct_load\.cgs/;
    open(my $CGS_OUT, ">", $cgs_dl_file) || die "ERROR: Cannot open $cgs_dl_file, $!";
    print $CGS_OUT @dl_lines;
    close $CGS_OUT;

    generate_direct_load_hex($cgs_dl_file, \@dl_lines);

    open($CGS_OUT, ">", $cgs_file) || die "ERROR: Cannot open $cgs_file, $!";
    print $CGS_OUT @ds_lines;
    close $CGS_OUT;
}

sub generate_direct_load_hex
{
    my ($file, $dl_lines) = @_;
    my $bracket_count=0;
    my $addr = 0;
    my $addr_high = 0;
    my $in_direct_load = 0;
    $file =~ s/\.cgs$/\.hex/;

    open(my $HEX, '>', $file) or die "Cannot open $file for write: $!\n";
    binmode $HEX;

    foreach my $line (@{$dl_lines}) {
        $bracket_count++ if $line =~ /^\s*\{/;
        $bracket_count-- if $line =~ /^\s*\}/;
        if($bracket_count == 0) {
            $in_direct_load = $line =~ /^\s*DIRECT_LOAD\s*=/;
        }
        if($in_direct_load) {
            if($line =~ /^\s*\"Address\"\s*=\s*(0x[0-9a-fA-F]+)/) {
                $addr = hex($1);
                if(($addr >> 16) != $addr_high) {
                    $addr_high = $addr >> 16;
                    print $HEX hex_record(0, 4, sprintf("%04X", $addr_high)), "\n";
                }
            }
            elsif($bracket_count == 2 && $line =~ /^\s*[0-9a-fA-F ]+/) {
                $line =~ s/\s//g;
                print $HEX hex_record($addr & 0xffff, 0, $line), "\n";
                $addr += length($line)/2;
                if(($addr >> 16) != $addr_high) {
                    $addr_high = $addr >> 16;
                    print $HEX hex_record(0, 4, sprintf("%04X", $addr_high)), "\n";
                }
            }
        }
    }
    print $HEX ":00000001FF\n";

    close $HEX;
}

sub hex_record
{
    my ($addr, $type, $data) = @_;

    my $record = sprintf "%02X%04X%02X", length($data)/2, $addr, $type;
    $record .= uc($data);
    my $checksum = 0;
    $checksum += $_ for unpack('C*', pack("H*", $record));
    my $hex_sum = sprintf "%04X", $checksum;
    $hex_sum = substr($hex_sum, -2); # just save the last byte of sum
    $checksum = (hex($hex_sum) ^ 0xFF) + 1; # 2's complement of hex_sum
    $checksum &= 0xff;
    $checksum = sprintf "%02X", $checksum; # convert checksum to string
    $record = ":" . $record . $checksum;
    return $record;
}
