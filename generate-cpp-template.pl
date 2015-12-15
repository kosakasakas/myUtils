#!/usr/bin/perl

use strict;
use warnings;
use Getopt::Long;
use utf8;
use Encode::Guess qw/utf8/;

sub write_template {
    my %arg = @_;
    my ($dir, $file_name, $text) = @arg{'dir', 'file_name', 'text'};

    my $file_path = $dir.$file_name;
    if( -f $file_path ) {
        print "$file_name is already exist. going to delete old one.\n";
        unlink $file_path;
        print "$file_name is deleted!\n";
    }
    open(FH, ">", $file_path) || die "cannot open: $!";
    print "writing file: $file_path\n";
    print FH $text;
    print "done.\n";
    close FH;
}

sub make_text {
    my %arg = @_;
    my ($type, $class, $right, $username, $summary, $namespace)
    = @arg{'type', 'class', 'right', 'username', 'summary', 'namespace'};

    my ($day,$mon,$year) = (localtime(time))[3..5];
    $year += 1900;
    $mon += 1;

    my $file_name = $class . "." . $type;
    my $text;

    $text .= <<"EOT";
//
//  $file_name
//
EOT
    $text .= !(defined $summary) ? "" : "//  ${summary}\n";
    $text .= "//\n";
    if (defined $username)
    {
        $text .= "//  Created by $username on $year/$mon/$day.\n";
    }
    else
    {
        $text .= "//  Created on $year/$mon/$day.\n";
    }
    if (defined $right)
    {
        $text .= "//  Copyright (c) ${year}年 $right. All rights reserved.\n"
    } 
    elsif (defined $username) 
    {
        $text .= "//  Copyright (c) ${year}年 $username. All rights reserved.\n"
    }
    $text .= "//\n\n";

    if ($type eq "inl" || $type eq "h")
    {
        $text .= <<"EOT";
#ifndef ___INCLUDE_${class}_${type}___
#define ___INCLUDE_${class}_${type}___

EOT
    }
    $text .= "// File Dependencies\n\n";

    if ($type eq "cpp" || $type eq "inl") 
    {
        $text .= "// Implementations\n";
    }
    else
    {
        $text .= "// Function Definitions\n";
    }

    if (defined $namespace)
    {
        if ($type eq "cpp")
        {
            $text .= "using namespace $namespace;\n\n";
        }
        else
        {
            $text .= <<"EOT";
namespace $namespace {
}// namespace $namespace {

EOT
        }
    }
    else
    {
        $text .= "\n";
    }

    if ($type eq "h" || $type eq "inl")
    {
        $text .= "#endif /* defined(___INCLUDE_${class}_${type}___) */\n";
    }

    $text .= "/* End Of File */\n\n";
    return $text;
}

sub generate_template {
	my $options = shift;

	my $class = $options->{class};
	my $type = $options->{type};
	my $namespace = $options->{namespace};
	my $username = $options->{username};
	my $dir = $options->{dir};
	my $right = $options->{right};
	my $summary = $options->{summary};
    $summary = Encode::decode_utf8($summary);

    $dir = ($dir =~ /\/$/) ? $dir : $dir."\/";

    # ヘッダファイルを作成
    my %arg = (
        type => "h",
        class => $class,
        right => $right,
        username => $username,
        summary => $summary,
        namespace => $namespace
    );
    my $text = &make_text( %arg );

    %arg = (
        dir => $dir,
        file_name => $class . ".h",
        text => $text
    );
    &write_template( %arg );

    # 実装ファイルを作成
    if ($type eq "cpp" || $type eq "inl") {
        %arg = (
            type => $type,
            class => $class,
            right => $right,
            username => $username,
            summary => $summary,
            namespace => $namespace
        );
        my $text = &make_text( %arg );
        my $file_path = $class . "." . $type;
        %arg = (
            dir => $dir,
            file_name => $file_path,
            text => $text
        );
        &write_template( %arg );
    }

	print "complated.\n";
}

my %options = (
	type => 'cpp',
	class => 'MyClass',
	summary => undef,
	right => undef,
	namespace => undef,
	username => undef,
	dir => ".\/"
);

GetOptions(\%options, 'type=s', 'class=s', 'username=s', 'dir=s', 'summary=s', 'right=s', 'namespace=s');
generate_template(\%options);
