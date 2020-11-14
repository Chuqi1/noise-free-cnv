/*
 *      noise-free-cnv-filter.cc - this file is part of noise-free-cnv.
 *
 *      Copyright 2010-2013 Philip Ginsbach <philip.develop@gmail.com>
 *
 *      This program is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "GtkCnvInterface.hh"
#include "CnvOperations.hh"
#include "CnvLoadSave.hh"
#include "PennCnvLoadSave.hh"
#include "CnvEncodeDecode.hh"
#include <gtkmm.h>
#include <iostream>
#include <fstream>
#include "CnvCalling.hh"

#if !(defined(NFCNV_VERSION_MAJOR)&&defined(NFCNV_VERSION_MINOR))
#include "config.h"
#endif

/* The noise-free-cnv-filter program is used to automatically reduce the noise
   in several PennCNV data files by identifying systematic variations in the
   examined files and removing them. This program has been adjusted to concrete
   problems and is not really usable as a general purpose program without
   changing the code. */

Cnv::Sequence normalize_sequence(const Cnv::Sequence& s, double& x_chr)
{
	double autosome_average=0.0, gonosome_average=0.0;
	unsigned autosome_divisor=0, gonosome_divisor=0;
	for(Cnv::SequenceSingleIterator iter(s); iter; ++iter)
	{
		std::string id;
		unsigned char chr;
		unsigned pos;
		Cnv::decompose_point_name(iter.name(), id, chr, pos);

		if(chr<251&&!std::isnan(iter.value())&&!std::isinf(iter.value()))
		{
			autosome_average+=iter.value();
			autosome_divisor++;
		}
		else if(chr==251&&!std::isnan(iter.value())&&!std::isinf(iter.value()))
		{
			gonosome_average+=iter.value();
			gonosome_divisor++;
		}
	}
	if(autosome_divisor!=0) autosome_average/=(double)autosome_divisor;
	if(gonosome_divisor!=0) gonosome_average/=(double)gonosome_divisor;

	Cnv::Sequence new_sequence;
	for(Cnv::SequenceSingleIterator iter(s); iter; ++iter)
	{
		std::string id;
		unsigned char chr;
		unsigned pos;
		Cnv::decompose_point_name(iter.name(), id, chr, pos);

		if(chr<251) new_sequence.push_back(
			iter.name(), iter.value()-autosome_average);
		else if(chr==251) new_sequence.push_back(
			iter.name(), iter.value()-gonosome_average);
	}
	x_chr=gonosome_average;
	return new_sequence;
}

Cnv::Sequence unnormalize_x_chromo(const Cnv::Sequence& s, double x_chr)
{
	Cnv::Sequence new_sequence;
	for(Cnv::SequenceSingleIterator iter(s); iter; ++iter)
	{
		std::string id;
		unsigned char chr;
		unsigned pos;
		Cnv::decompose_point_name(iter.name(), id, chr, pos);

		if(chr==251) new_sequence.push_back(
			iter.name(), iter.value()+x_chr);
		else new_sequence.push_back(iter.name(), iter.value());
	}
	return new_sequence;
}

int main(int Args, char** Arg)
{
	bool verbose = false;
	bool only_profiles = false;
	bool use_sex_chromosomes = false;
	std::string  low_profile_file;
	std::string  high_profile_file;
	std::vector<std::string> filenames;

	if(Args<2)
	{
		std::cout<<"Usage: noise-free-cnv-filter [OPTION]... [FILE]...\n"
			"Remove shared noise in multiple files of DNA-microarray data (PennCNV format)\n"
			"\n"
			"  -h, --help                    display this help and exit\n"
			"      --version                 output version information and exit\n"
			"  -v, --verbose                 verbosely print progress and statistics\n"
			"      --wave-profile [FILE]     use precomputed wave profile\n"
			"      --per-snp-profile [FILE]  use precomputed per-SNP profile\n"
			"      --use-sex-chromosomes     do not discard sex chromosomes\n"
			"      --only-profiles           do not apply the profiles\n"
			"\n"
			"Report noise-free-cnv bugs to philip.development@googlemail.com\n"
			"noise-free-cnv home page: <http://noise-free-cnv.sourceforge.net>"<<std::endl;
		return 0;
	}
	for(int i=1; i<Args; i++)
	{
		if(!strcmp(Arg[i], "-h")||!strcmp(Arg[i], "--help"))
		{
			std::cout<<"Usage: noise-free-cnv-filter [OPTION]... [FILE]...\n"
				"Remove shared noise in multiple files of DNA-microarray data (PennCNV format)\n"
			"\n"
			"  -h, --help                    display this help and exit\n"
			"      --version                 output version information and exit\n"
			"  -v, --verbose                 verbosely print progress and statistics\n"
			"      --wave-profile [FILE]     use precomputed wave profile\n"
			"      --per-snp-profile [FILE]  use precomputed per-SNP profile\n"
			"      --use-sex-chromosomes     do not discard sex chromosomes\n"
			"      --only-profiles           do not apply the profiles\n"
			"\n"
				"Report noise-free-cnv bugs to philip.development@googlemail.com\n"
				"noise-free-cnv home page: <http://noise-free-cnv.sourceforge.net>"<<std::endl;
			return 0;

		}
		else if(!strcmp(Arg[i], "--version"))
		{
			std::cout<<"noise-free-cnv-filter "<<NFCNV_VERSION_MAJOR<<"."<<NFCNV_VERSION_MINOR<<"\n"
				"Copyright (c) 2010, 2011, 2012 Philip Ginsbach\n"
				"License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n"
				"This is free software: you are free to change and redistribute it.\n"
				"There is NO WARRANTY, to the extent permitted by law.\n"
				"\n"
				"Written by Philip Ginsbach."<<std::endl;
			return 0;
		}
		else if(!strcmp(Arg[i], "--wave-profile"))
		{
			if(++i<Args)
			{
				low_profile_file = std::string(Arg[i]);
			}
		}
		else if(!strcmp(Arg[i], "--per-snp-profile"))
		{
			if(++i<Args)
			{
				high_profile_file = std::string(Arg[i]);
			}
		}
		else if(!strcmp(Arg[i], "--only-profiles"))
		{
			only_profiles = true;
		}
		else if(!strcmp(Arg[i], "--use-sex-chromosomes"))
		{
			use_sex_chromosomes = true;
		}
		else if(!strcmp(Arg[i], "-v")||!strcmp(Arg[i], "--verbose"))
		{
			verbose=true;
		}
		else if(Arg[i][0]=='-')
		{
			std::cout<<"noise-free-cnv-filter: unrecognized option \'"<<Arg[i]<<"\'\n"
				"Try \'noise-free-cnv-filter --help\' for more information."<<std::endl;
			return 0;
		}
		else
		{
			filenames.push_back(Arg[i]);
		}
	}

	if(verbose) std::cout<<"used flags: "<<(verbose?"verbose ":"")<<(only_profiles?"only-profiles":"")<<(use_sex_chromosomes?"use_sex_chromosomes":"")<<std::endl;

	Cnv::StringPool string_pool;
	std::vector<const Cnv::Sequence*> temp_vector;

	Cnv::Sequence low_profile;
	
	if(low_profile_file.empty())
	{
		if(verbose) std::cout<<"computing wave profile: "<<std::endl;

		std::vector<Cnv::Sequence> low_seq_vec;
		low_seq_vec.reserve(Args-1);
		for(unsigned i=0; i<filenames.size(); i++)
		{
			if(verbose) std::cout<<"  file \'"<<filenames[i]<<"\' ...";
			if(verbose) std::cout.flush();

			std::vector<Cnv::Sequence> pair=
				PennCnv::load(filenames[i], string_pool);

			double X_chr_intens=0.0;
			if( !use_sex_chromosomes ) pair[0]=Cnv::stripXY(pair[0]);
			pair[0]=normalize_sequence(pair[0], X_chr_intens);
			pair[0]=Cnv::blur(pair[0], 1000.0);

			low_seq_vec.push_back(pair[0]);

			if(verbose) std::cout<<" done"<<std::endl;
		}

		if(verbose) std::cout<<"  computing median sequence ...";
		if(verbose) std::cout.flush();

		temp_vector.resize(low_seq_vec.size());
		for(unsigned k=0; k<low_seq_vec.size(); k++)
			temp_vector[k]=&(low_seq_vec[k]);
		low_profile=Cnv::median(temp_vector);

		if(verbose) std::cout<<"saving as \"wave_profile\": "<<std::endl;

		Cnv::save(low_profile, "wave_profile");

		if(verbose) std::cout<<" done!"<<std::endl;
	}
	else
	{
		if(verbose) std::cout<<"loading wave profile: "<<std::endl;

		low_profile = Cnv::load( low_profile_file, string_pool );

		if(verbose) std::cout<<" done!"<<std::endl;
	}


	Cnv::Sequence high_profile;
	if(high_profile_file.empty())
	{
		if(verbose) std::cout<<"computing per-SNP profile: "<<std::endl;

		std::vector<Cnv::Sequence> high_seq_vec;
		high_seq_vec.reserve(Args-1);
		for(unsigned i=0; i<filenames.size(); i++)
		{
			if(verbose) std::cout<<"  file \'"<<filenames[i]<<"\' ...";
			if(verbose) std::cout.flush();

			std::vector<Cnv::Sequence> pair=
				PennCnv::load(filenames[i], string_pool);

			double X_chr_intens=0.0;
			if( !use_sex_chromosomes ) pair[0]=Cnv::stripXY(pair[0]);
			pair[0]=normalize_sequence(pair[0], X_chr_intens);
			pair[0]=pair[0]-Cnv::blur(pair[0], 1000.0);

			high_seq_vec.push_back(pair[0]);

			if(verbose) std::cout<<" done"<<std::endl;
		}

		if(verbose) std::cout<<"  computing median sequence ...";
		if(verbose) std::cout.flush();

		temp_vector.resize(high_seq_vec.size());
		for(unsigned k=0; k<high_seq_vec.size(); k++)
			temp_vector[k]=&(high_seq_vec[k]);
		high_profile=Cnv::median(temp_vector);

		if(verbose) std::cout<<"saving as \"per-snp_profile\": "<<std::endl;

		Cnv::save(high_profile, "per-snp_profile");

		if(verbose) std::cout<<" done!"<<std::endl;
	}
	else
	{
		if(verbose) std::cout<<"loading per-SNP profile: "<<std::endl;

		high_profile = Cnv::load( high_profile_file, string_pool );

		if(verbose) std::cout<<" done!"<<std::endl;
	}


	if(!only_profiles)
	{
		if(verbose) std::cout<<"Applying profiles:"<<std::endl;
		if(verbose) std::cout<<"filename\tvariance\twave variance\tper-SNP variance\twave profile variance\tper-SNP profile variance\t"
			"wave covariance\tper-SNP covariance\twave correlation\tper-SNP correlation\twave subtraction factor\tper-SNP subtraction factor"<<std::endl;

		for(unsigned i=0; i<filenames.size(); i++)
		{
			if(verbose) std::cout<<filenames[i]<<"\t";
			if(verbose) std::cout.flush();

			std::vector<Cnv::Sequence> pair=
				PennCnv::load(filenames[i], string_pool);

			double X_chr_intens=0.0;
			if( !use_sex_chromosomes ) pair[0]=Cnv::stripXY(pair[0]);

			Cnv::Sequence whole_seq = normalize_sequence(pair[0], X_chr_intens);
			Cnv::Sequence low_seq   = Cnv::blur(whole_seq, 1000.0);
			Cnv::Sequence high_seq  = whole_seq-low_seq;

			Cnv::Sequence whole_var = Cnv::avg(whole_seq*whole_seq);
			Cnv::Sequence low_var   = Cnv::avg(low_seq*low_seq);
			Cnv::Sequence high_var  = Cnv::avg(high_seq*high_seq);

			Cnv::Sequence low_prof_var   = Cnv::avg(low_profile*low_profile);
			Cnv::Sequence high_prof_var  = Cnv::avg(high_profile*high_profile);

			Cnv::Sequence low_covar   = Cnv::avg(low_seq*low_profile);
			Cnv::Sequence high_covar  = Cnv::avg(high_seq*high_profile);

			Cnv::Sequence low_correl   = low_covar / Cnv::pow( low_var*low_prof_var, 0.5 );
			Cnv::Sequence high_correl  = high_covar / Cnv::pow( high_var*high_prof_var, 0.5 );

			Cnv::Sequence low_factor   = low_covar / low_prof_var;
			Cnv::Sequence high_factor  = high_covar / high_prof_var;

			low_seq  = low_seq-low_profile * low_covar / low_prof_var;
			high_seq = high_seq-high_profile * high_covar / high_prof_var;
			pair[0]  = unnormalize_x_chromo(low_seq+high_seq, X_chr_intens);

			if(verbose) std::cout
				<<whole_var.get_values()[0]<<"\t"
				<<low_var.get_values()[0]<<"\t"
				<<high_var.get_values()[0]<<"\t"
				<<low_prof_var.get_values()[0]<<"\t"
				<<high_prof_var.get_values()[0]<<"\t"
				<<low_covar.get_values()[0]<<"\t"
				<<high_covar.get_values()[0]<<"\t"
				<<low_correl.get_values()[0]<<"\t"
				<<high_correl.get_values()[0]<<"\t"
				<<low_factor.get_values()[0]<<"\t"
				<<high_factor.get_values()[0];

			PennCnv::save(pair, filenames[i]+".nf");

			if(verbose) std::cout<<std::endl;
		}

		if(verbose) std::cout<<"done!"<<std::endl;
	}

	return 0;
}
