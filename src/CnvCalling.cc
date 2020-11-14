/*
 *      CnvCalling.hh - this File is part of noise-free-cnv.
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

#include "CnvCalling.hh"
#include <vector>
#include <list>
#include <string>
#include <cmath>
#include <iostream>
#include <limits>

#include "CnvLoadSave.hh"
#include "CnvEncodeDecode.hh"
#include "CnvCallingReport.hh"

/* This file implements calling algorithms to autonomously find copy number
   variations in DNA-microarray data sequences. 

   The calling part of noise-free-cnv is largely experimental and not included
   in the GUI program noise-free-cnv-gtk. */

namespace Cnv { namespace Call {

Configuration::Configuration()
	:verbose(false),search_delets(true),search_duplis(true),
	delet_expect(-0.58),dupli_expect(0.58),
	border_width(8),max_width(2048),
	trigger_threshold(0.58*0.5),final_threshold(0.5){}

float compute_median(std::vector<float>& vec)
{
	if(vec.size()==0) return 0.0;
	else if((vec.size()%2)==1)
	{
		std::nth_element(vec.begin(),
			vec.begin()+vec.size()/2, vec.end());
		return vec[vec.size()/2];
	}
	else
	{
		std::nth_element(vec.begin(),
			vec.begin()+vec.size()/2, vec.end());
		float temp=vec[vec.size()/2];

		std::nth_element(vec.begin()+vec.size()/2+1,
			vec.begin()+vec.size()/2+1, vec.end());
		return (temp+vec[vec.size()/2+1])/2.0;
	}
}

float compute_median(std::vector<float>::const_iterator it1,
	std::vector<float>::const_iterator it2)
{
	std::vector<float> temp(it1, it2);
	return compute_median(temp);
}

double compute_average_gradient(bool decrease_increase,
	std::vector<float>::const_iterator begin,
	std::vector<float>::const_iterator end)
{
	if(begin==end) return 0.0;

	unsigned width=std::distance(begin, end);
	unsigned max_pos;
	if(decrease_increase) max_pos=0;
	else max_pos=width;

	double weighted_sum=0.0;
	double devisor=0.0;
	for(unsigned i=0; i<std::distance(begin, end); i++)
	{
		double weight;
		if(i<max_pos) weight=(double)(2*i+1)/(double)(2*max_pos);
		else weight=(double)(2*(max_pos-i)-1)/(double)(2*max_pos);

		weight=weight*weight;
		weighted_sum+=weight*begin[i];
		devisor+=weight;
	}

	return weighted_sum/devisor;
}

long double gaussian_bell(double x, double dev)
{
	return ::exp((long double)(-0.5*x*x/(dev*dev)));
}

double compute_left_jump(const Configuration& config,
	std::vector<float>::const_iterator begin,
	std::vector<float>::const_iterator center,
	std::vector<float>::const_iterator end)
{
	double first=compute_average_gradient(false, begin, center);
	double second=compute_average_gradient(true, center, end);
	return second-first;
}

double compute_right_jump(const Configuration& config,
	std::vector<float>::const_iterator begin,
	std::vector<float>::const_iterator center,
	std::vector<float>::const_iterator end)
{
	double first=compute_average_gradient(false, begin, center);
	double second=compute_average_gradient(true, center, end);
	return first-second;
}

long double compute_left_score(const std::vector<double>& deviation,
	std::vector<float>::const_iterator begin,
	std::vector<float>::const_iterator center,
	std::vector<float>::const_iterator end,
	bool dupli_delet, const Configuration& config)
{
	double jump=compute_left_jump(config, begin, center, end);

	long double normal_score=gaussian_bell(jump,
		deviation[std::distance(center,end)-1]);

	long double delet_score=gaussian_bell(jump-config.delet_expect,
		deviation[std::distance(center,end)-1]);

	long double dupli_score=gaussian_bell(jump-config.dupli_expect,
		deviation[std::distance(center,end)-1]);

	if(dupli_delet)
		return normal_score*0.99/(normal_score*0.99+dupli_score*0.01);
	else return normal_score*0.99/(normal_score*0.99+delet_score*0.01);
}

long double compute_right_score(const std::vector<double>& deviation,
	std::vector<float>::const_iterator begin,
	std::vector<float>::const_iterator center,
	std::vector<float>::const_iterator end,
	bool dupli_delet, const Configuration& config)
{
	double jump=compute_right_jump(config, begin, center, end);

	long double normal_score=gaussian_bell(jump,
		deviation[std::distance(begin, center)-1]);

	long double delet_score=gaussian_bell(jump-config.delet_expect,
		deviation[std::distance(begin, center)-1]);

	long double dupli_score=gaussian_bell(jump-config.dupli_expect,
		deviation[std::distance(begin, center)-1]);

	if(dupli_delet)
		return normal_score*0.99/(normal_score*0.99+dupli_score*0.01);
	else return normal_score*0.99/(normal_score*0.99+delet_score*0.01);
}

std::vector<double> analyze_noise(const std::vector<float>& points,
	const Configuration& config)
{
	if(config.verbose) std::cout<<"analyzing noise ... 0%";
	if(config.verbose) std::cout.flush();

	std::vector<double> deviation;
	for(unsigned i=1; i<=config.max_width; i++)
	{
		unsigned new_samples=0;
		double new_deviation=0.0;

		for(unsigned j=0; j<1024/2; j++)
		{
			unsigned left=rand()%points.size();
			unsigned center1=left+i;
			unsigned center2=left+i;
			unsigned right=center1+i;

			if(right<=points.size())
			{
				double jump_left=compute_left_jump(config,
					points.begin()+left, points.begin()+center1,
					points.begin()+right);

				double jump_right=compute_right_jump(config,
					points.begin()+left, points.begin()+center2,
					points.begin()+right);

				new_samples+=2;
				new_deviation+=jump_left*jump_left
					+jump_right*jump_right;
			}
		}

		new_deviation=::sqrt(new_deviation/(double)new_samples);

		if(deviation.size()==0||deviation.back()>new_deviation)
			deviation.push_back(new_deviation);
		else deviation.push_back(deviation.back());

		if(config.verbose&&i*100/config.max_width>0
			&&i*100/config.max_width!=(i-1)*100/config.max_width)
		{
			unsigned percentage=i*100/config.max_width;
			std::cout<<"\ranalyzing noise ... "<<percentage<<"%";
			std::cout.flush();
		}
	}

	if(config.verbose) std::cout<<"\ranalyzing noise ... done\n";
	return deviation;
}

std::vector<double> first_pass(const std::vector<float>& points,
	bool dupli_delet, const Configuration& config)
{
	if(config.verbose) std::cout<<"first pass ... 0%";
	if(config.verbose) std::cout.flush();

	std::vector<double> grid; grid.resize(points.size());
	for(unsigned i=0; i<points.size(); i++)
	{
		unsigned start=(i>=2)?(i-2):(0);
		unsigned end=(i+3<=points.size())?(i+3):(points.size());
		grid[i]=compute_median(points.begin()+start, points.begin()+end);

		if(config.verbose&&i*100/points.size()>0
			&&i*100/points.size()!=(i-1)*100/points.size())
		{
			unsigned percentage=i*100/points.size();
			std::cout<<"\rfirst pass ... "<<percentage<<"%";
			std::cout.flush();
		}
	}

	if(config.verbose) std::cout<<"\rfirst pass ... done\n";

	return grid;
}

void calling(const Sequence& seq, bool dupli_delet,
	std::vector<ReportEntry>& report, const Configuration& config)
{
	const std::vector<float>& points=seq.get_values();
	std::vector<double> deviation=analyze_noise(points, config);
	std::vector<double> grid=first_pass(points, dupli_delet, config);

	if(config.verbose) std::cout<<"second pass ... 0%";
	if(config.verbose) std::cout.flush();

	while(true)
	{
		std::vector<double>::iterator pos_elem;

		if(dupli_delet)
			pos_elem=std::max_element(grid.begin()+64, grid.end()-64);
		if(!dupli_delet)
			pos_elem=std::min_element(grid.begin()+64, grid.end()-64);

		if(dupli_delet&&*pos_elem<config.trigger_threshold) break;
		if(!dupli_delet&&*pos_elem>-config.trigger_threshold) break;

		if(config.verbose)
		{
			unsigned percentage;
			if(dupli_delet)
				percentage=(unsigned)(100.0*(1.0-*pos_elem)
					/(1.0-config.trigger_threshold));
			if(!dupli_delet)
				percentage=(unsigned)(100.0*(*pos_elem+1.0)
					/(1.0-config.trigger_threshold));
			if(percentage<100)
				std::cout<<"\rsecond pass ... "<<percentage<<"%";
			std::cout.flush();
		}

		std::vector<float>::const_iterator best_start=
			points.begin()+(pos_elem-grid.begin())-1;
		std::vector<float>::const_iterator best_end=
			points.begin()+(pos_elem-grid.begin())+2;

		size_t best_env_radius=best_end-best_start;

		long double best_left_score=compute_left_score(deviation,
			best_start-best_env_radius, best_start, best_end,
			dupli_delet, config);

		long double best_right_score=compute_right_score(deviation,
			best_start, best_end, best_end+best_env_radius,
			dupli_delet, config);

		std::vector<float>::const_iterator new_start=best_start;
		std::vector<float>::const_iterator new_end=best_end;

		while(new_end-best_start<config.max_width
			&&best_end-new_start<config.max_width)
		{
			bool expansion_possible=false;
			if(new_start>points.begin()+config.border_width
				&&grid[new_start-points.begin()-1]!=DBL_MAX
				&&grid[new_start-points.begin()-1]!=-DBL_MAX)
			{
				new_start--;
				expansion_possible=true;
			}
			if(new_end+config.border_width<points.end()
				&&grid[new_end-points.begin()]!=DBL_MAX
				&&grid[new_end-points.begin()]!=-DBL_MAX)
			{
				new_end++;
				expansion_possible=true;
			}
			if(!expansion_possible) break;

			unsigned new_env_radius=new_end-best_start;
			if(new_env_radius>new_start-points.begin())
				new_env_radius=new_start-points.begin();
			if(new_env_radius>points.end()-new_end)
				new_env_radius=points.end()-new_end;

			long double new_left_score1=compute_left_score(deviation,
				new_start-new_env_radius, new_start, best_end,
				dupli_delet, config);

			long double new_right_score1=compute_right_score(deviation,
				new_start, best_end, best_end+new_env_radius,
				dupli_delet, config);

			long double new_left_score2=compute_left_score(deviation,
				best_start-new_env_radius, best_start, new_end,
				dupli_delet, config);

			long double new_right_score2=compute_right_score(deviation,
				best_start, new_end, new_end+new_env_radius,
				dupli_delet, config);

			if(best_start!=new_start&&new_left_score1<best_left_score
				&&new_left_score1-best_left_score
					<new_right_score2-best_right_score)
			{
				best_start=new_start;
				new_end=best_end;
				best_left_score=new_left_score1;
				best_right_score=new_right_score1;
			}
			else if(best_end!=new_end&&new_right_score2<best_right_score)
			{
				best_end=new_end;
				new_start=best_start;
				best_left_score=new_left_score2;
				best_right_score=new_right_score2;
			}
		}

		for(unsigned j=0; j<best_end-best_start; ++j)
		{
			if(dupli_delet) grid[best_start-points.begin()+j]=-DBL_MAX;
			if(!dupli_delet) grid[best_start-points.begin()+j]=DBL_MAX;
		}

		if((1.0-best_left_score)*(1.0-best_right_score)>=config.final_threshold)
		{
			std::string id1, id2, chr1, chr2, pos1, pos2;
			decompose_point_name(seq.get_names()[best_start-points.begin()], id1, chr1, pos1);
			decompose_point_name(seq.get_names()[best_end-points.begin()-1], id2, chr2, pos2);

			ReportEntry entry;
			if(dupli_delet) entry.type=3;
			else entry.type=1;

			entry.start_number=best_start-points.begin();
			entry.end_number=best_end-points.begin()-1;
			entry.chromosome=decode_chr(chr1.begin(), chr1.end());
			entry.start_id=id1;
			entry.end_id=id2;
			entry.start_pos=decode_pos(pos1.begin(), pos1.end());
			entry.end_pos=decode_pos(pos2.begin(), pos2.end());
			entry.score_inner=(1.0-best_left_score)*(1.0-best_right_score);
			entry.score_enter=1.0-best_left_score;
			entry.score_leave=1.0-best_right_score;

			report.push_back(entry);
		}
	}

	if(config.verbose) std::cout<<"\rsecond pass ... done\n";
}

void calling(const Sequence& seq,
	std::vector<ReportEntry>& report, const Configuration& config)
{
	if(config.search_delets) calling(seq, false, report, config);
	if(config.search_duplis) calling(seq, true, report, config);
}


}}
