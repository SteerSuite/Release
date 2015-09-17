//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/*
 * CompositeTechniqueEntropy.cpp
 *
 *  Created on: 2013-09-22
 *      Author: glenpb
 *
 *  Example: ../build/bin/steersim -module scenario,scenarioAI=rvo2dAI,useBenchmark,benchmarkTechnique=compositeEntropy,benchmarkLog=data//0/test.log,checkAgentValid,reducedGoals,fixedSpeed,checkAgentRelevant,minAgents=3,ailogFileName=data//0/pprAI.log,maxFrames=2000,checkAgentInteraction,egocentric,scenarioSetPath=data/RealWorldData/ou-060-180-180/,scenarioSetInitId=0,numScenarios=1,dbName=steersuitedb,skipInsert=True,RealDataName=data/RealWorldData/uo-050-180-180_combined_MB.txt -config configs/Entropy-config.xml
 *
 *  For replaying a piece of a dataset dataset
 *  ../build/bin/steersim -module scenario,scenarioAI=rvo2dAI,useBenchmark,benchmarkTechnique=compositeEntropy,benchmarkLog=data//0/test.log,checkAgentValid,reducedGoals,fixedSpeed,checkAgentRelevant,minAgents=3,ailogFileName=data//0/pprAI.log,maxFrames=2000,checkAgentInteraction,egocentric,scenarioSetPath=data/RealWorldData/ou-060-180-180/,scenarioSetInitId=0,numScenarios=1,dbName=steersuitedb,skipInsert=True,RealDataName=data/RealWorldData/uo-300-300-200_combined_MB.txt,replayData,perferedNumAgents=10,recFile=glah.rec -config configs/Entropy-config.xml
 */


#include "benchmarking/CompositeTechniqueEntropy.h"
#include "interfaces/EngineInterface.h"
#include "testcaseio/AgentInitialConditions.h"
#include "benchmarking/BayesianFilter.h"



#include <fstream>

// #define _DEBUG_ENTROPY 1

using namespace SteerLib;

CompositeTechniqueEntropy::CompositeTechniqueEntropy()
{
	// TODO Auto-generated constructor stub

}

CompositeTechniqueEntropy::~CompositeTechniqueEntropy()
{
	// TODO Auto-generated destructor stub
}

/*
void CompositeTechniqueEntropy::preprocessFrame(float currentSimulationTime, float simulatonDt, unsigned int currentFrameNumber)
{
	std::cout << "Grr" << std::endl;
}
*/

void CompositeTechniqueEntropy::init()
{
	std::cout << "Entropy has been init()ed" << std::endl;
	sx = 4;
	sumSamp = 2;
	_numSamples = 200;
	_invNumSamples = (1/(float)_numSamples); // mostly for efficiency
	std::cout << "inverse Num Samples " << _invNumSamples << std::endl;
	curStep = 0;
	float fps = 16.0f;
	_timeStep = 1/fps;
	// FOr now this need to be set so that vectors can be resized.
	_numAgt = 0; // scalling down for quicker testing turn around
	_estimationDone = 0;
	// _agentStartFrame.resize(_numAgt);

	// real world data is in cm
	_realWorldDataScale = 0.01f;
	_inverseScale = 2.0f;
	_f_hat_calls = 0;

	_perferedNumAgents=10;
	_perferedNumFrames=((1/_timeStep)*4);
	_replay_data=0;

}

Matrix CompositeTechniqueEntropy::h(const Matrix & x, const Matrix & n)
{
	Matrix z(Z_DIM);
	for (unsigned a = 0; a < _numAgt; a++)
	{
		z[2*a] = x[sx*a] + (_invNumSamples*n[2*a]);
		z[2*a+1] = x[sx*a+1] + (_invNumSamples*n[2*a+1]);
	}
	return z;
}

void CompositeTechniqueEntropy::m_initGuess(Matrix &Sigma, Matrix &xHat)
{
	int sx = 4;
	for (int a = 0; a < _numAgt; a++)
	{
		Sigma(sx*a+0,sx*a+0) = _timeStep/2;
		Sigma(sx*a+1,sx*a+1) = _timeStep/2;
		Sigma(sx*a+2,sx*a+2) = _timeStep/2;
		Sigma(sx*a+3,sx*a+3) = _timeStep/2;
		xHat[sx*a+0] = _posData[a][0].x;
		xHat[sx*a+1] = _posData[a][0].y;
		// Need to be scaled up to the proper velocity
		xHat[sx*a+2] = (_posData[a][1].x-_posData[a][0].x)/_timeStep;
		xHat[sx*a+3] = (_posData[a][1].y-_posData[a][0].y)/_timeStep;
	}
}

void CompositeTechniqueEntropy::init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo )
{
	init();

	SteerLib::OptionDictionary::const_iterator optionIter;
	for (optionIter = options.begin(); optionIter != options.end(); ++optionIter)
	{
		if ((*optionIter).first == "RealDataName")
		{
			std::cout << "Setting dataFile name: " << (*optionIter).second << std::endl;
			_realDataFileName = (*optionIter).second;
		}
		else if ( (*optionIter).first == "scenarioAI")
		{
			_agentModuleName = (*optionIter).second;
		}
		else if ( (*optionIter).first == "perferedNumAgents")
		{
			_perferedNumAgents = atoi((*optionIter).second.c_str());
		}
		else if ( (*optionIter).first == "perferedNumFrames")
		{
			_perferedNumFrames = atoi((*optionIter).second.c_str());
		}
		else if ( (*optionIter).first == "numEntropySamples")
		{

			_numSamples = atoi((*optionIter).second.c_str());
			std::cout << "Number of samples has been set to " << _numSamples << std::endl;
		}
		else if ( (*optionIter).first == "replayData")
		{

			_replay_data = true;
			std::cout << "Replaying real world data" << std::endl;
		}

		else
		{
			// throw Util::GenericException("unrecognized option \"" + Util::toString((*optionIter).first) + "\" given to steerBench module.");
		}
	}

	this->setEngineInterface(engineInfo);

	_agentModule = this->getEngineInterface()->getModule(_agentModuleName);
	std::cout << "moduleInterface " << _agentModule << std::endl;
	// Agents are not ready yet;
	// this->performEstimation();
}

/*
 * Depreciated
 */
void CompositeTechniqueEntropy::readNoisyData(std::string fileName)
{

	std::ifstream infile(fileName, std::ifstream::in);

	if ( !infile.eof() && ( infile.fail() || infile.bad() ) )
	{
		std::cout << "error reading file position data file " << fileName
			 << " error: " << strerror( errno ) << std::endl;
		// return;
	}
	// std::cout << "file thing: " << infile << " good? " << infile.good() << " error: " << strerror( errno ) << std::endl;
	int agentNumber, frameNumber;
	float x, y, z;
	mPair p;
	/*
	 * posData[agent][point] I think
	 */
	while ( infile >> agentNumber >> frameNumber >> x >> y >> z)
	{

		if ( this->_posData[agentNumber-1].size() < 100 )
		{
			p.x = x * _realWorldDataScale;
			p.y = y * _realWorldDataScale;
			// ignore Z in this case


			this->_posData[agentNumber-1].push_back(p);
			/*
			 * I guess this also assumes the data is read in the order it is
			 * supposed simulated.
			 */

			if ( _agentStartFrame[agentNumber-1] == 0)
			{
				_agentStartFrame[agentNumber-1] = frameNumber;
			}
		}
	}

	// This will adjust the simulation to start at the begining of the simulation
	int timeadjust = _agentStartFrame[0];
	for (int a = 0; a < _numAgt; a++)
	{

		_agentStartFrame[a] = _agentStartFrame[a] - timeadjust;
		// std::cout << "agent: " << a << " has " << this->_posData[a].size() <<
			// 	" and starts at frame " << _agentStartFrame[a] << std::endl;
	}

}

int CompositeTechniqueEntropy::readNoisyData2(std::string fileName)
{

	std::ifstream infile(fileName, std::ifstream::in);

	if ( !infile.eof() && ( infile.fail() || infile.bad() ) )
	{
		std::cout << "error reading file position data file " << fileName
			 << " error: " << strerror( errno ) << std::endl;
		// return;
	}
	// std::cout << "file thing: " << infile << " good? " << infile.good() << " error: " << strerror( errno ) << std::endl;
	int agentNumber, frameNumber, numAgents = 0, maxFrameNumber = 0, minFrameNumber = 10000000;
	float x, y, z;
	mPair p;
	/*
	 * posData[agent][point] I think
	 */
	while ( infile >> agentNumber >> frameNumber >> x >> y >> z)
	{
		if ( agentNumber > _posData.size())
		{
			// increase data holder size.
			_posData.resize(agentNumber);
		}

		if ( this->_posData[agentNumber-1].size() < 1000 )
		{
			p.x = x * _realWorldDataScale;
			p.y = y * _realWorldDataScale;
			// ignore Z in this case

			// std::cout << agentNumber << " " << frameNumber << " x=" << x << std::endl;
			this->_posData[agentNumber-1].push_back(p);
			if (agentNumber > _numAgt)
			{
				numAgents = agentNumber;
			}
			if (maxFrameNumber < frameNumber)
			{
				maxFrameNumber = frameNumber;
			}
			if (minFrameNumber > frameNumber )
			{
				minFrameNumber = frameNumber;
			}
			/*
			 * I guess this also assumes the data is read in the order it is
			 * supposed simulated.
			 */
			if ( _agentStartFrame.size() < agentNumber)
			{
				_agentStartFrame.push_back(frameNumber);
			}
			/*
			if ( _agentStartFrame[agentNumber-1] == 0)
			{
				_agentStartFrame[agentNumber-1] = frameNumber;
			}*/
		}
	}

	// This will adjust the simulation to start at the begining When the
	// first agent appears.
	_numAgt = numAgents;
	int timeadjust = minFrameNumber;
	for (int a = 0; a < _numAgt; a++)
	{
		// Everything starts at frame 1
		_agentStartFrame[a] = (_agentStartFrame[a] - timeadjust) + 1;
		//std::cout << "agent: " << a << " has " << this->_posData[a].size() <<
			//	" and starts at frame " << _agentStartFrame[a] << std::endl;
	}

	// std::cout << "maxFrameNumber is " << maxFrameNumber << " and min is " << timeadjust << std::endl;
	return maxFrameNumber-timeadjust+1;
}

/**
 * This method is supposed to trim out data. It will do this to ensure a
 * good set of data is choosen. The final data for the produced should form a
 * rectangular data set agents x timesteps with the larges size
 *
 * similar to maximize multiprocess scheduling but we know more information about
 * the time of each process
 * for each timestep
 *
 * Create a matrix  Mt timesteps x timestepsahead where
 *
 * Mt[i][j] = sum of agents active from i to i+j
 *
 * The maximal item row will be the start frame for best data and the maximal
 * item column will be the time length from i.
 */
void CompositeTechniqueEntropy::selectGoodData(unsigned int maxFrameNumber)
{

	// real max is 1017 and min is 43
	std::cout << "max frame number is " << maxFrameNumber << std::endl;
	std::vector<std::vector<unsigned int> > dataSums;
	unsigned int timestart = 0, timestop = 0, maxsum = 0;

	dataSums.resize(maxFrameNumber);
	int i = 0;
	for (int i = 1; i < maxFrameNumber; i++ )
	{
		// calculate sum of agents active from i to i+j
		for (int j = i; j < maxFrameNumber; j++)
		{
			unsigned int sum = 0, acount = 0;
			for ( int a = 0; a < _numAgt; a++)
			{
				// is agent active at start           agent still active at end
				if ( (i >= _agentStartFrame[a]) && ((j) <= (_posData[a].size()+_agentStartFrame[a])) )
				{
					sum = sum + ((j - i)+1);
					// Keeps track of the number of agents in the simulation
					acount = acount + 1;
				}
			}
			// if ( (sum > maxsum) && (acount > 14) ) // agent min
			// if ( ((sum > maxsum) && (j-i == _perferedNumFrames )) && (acount == _perferedNumAgents) ) // time min 3s
			if ( ((sum > maxsum) && (j-i == _perferedNumFrames )))// && (acount == _perferedNumAgents) ) // time min 3s
			{
				// std::cout << "There are " << acount << " agent in this max" << std::endl;
				maxsum = sum;
				timestart = i;
				timestop = j;
			}
			acount = 0;
			dataSums[i].push_back(sum);
		}
	}

	/*
	for (int p = 0; p < dataSums.size(); p++)
	{
		std::cout << "dataSums[" << p << "].size() is = " << dataSums[p].size() << std::endl;
	}*/

	/*
	 * Reallocate timestep data according to best data subset
	 */
	std::cout << "Reallocating posData now with timestart: " << timestart << " and timestop: " << timestop << std::endl;
	std::cout << "numAgents: " << _numAgt << " and posData.size(): " << _posData.size() << std::endl;
	std::vector<std::vector<mPair>> tempPosData;

	tempPosData.resize(_numAgt);

	// for (int i = timestart; i < (timestop); i++ )
	{
		for ( int a = 0; a < _numAgt; a++)
		{
			if ( (timestart >= _agentStartFrame.at(a)) && ((timestop) <= (_posData.at(a).size() + _agentStartFrame.at(a)) ) )
			{
				// std::cout << "For agent " << a << " Adding point " << i-_agentStartFrame[a];
				// std::cout << " " << _posData[a].at(i-_agentStartFrame[a]) << " to tempPosData[" << a << "]" <<std::endl;
				tempPosData.at(a).assign(_posData.at(a).begin()+timestart-_agentStartFrame.at(a),
						_posData.at(a).begin()+((timestart-_agentStartFrame.at(a))+
								timestop-timestart));
			}
		}
	}

	/*
	for (int a = 0; a < _numAgt; a++)
	{
		std::cout << "agent: " << a << " has " << tempPosData.at(a).size() <<
				" and starts at frame " << _agentStartFrame.at(a) << std::endl;
	}*/

	std::vector<Util::Point> _tmpAgentGoals;
	/*
	 * This method uses the last position from original data.
	 */
	for (int a = 0; a < _numAgt; a++)
	{
		// std::cout << "at agent " << a << "pushing back goal " << Util::Vector(_posData.at(a).back().x, 0.0f, _posData.at(a).back().y) << std::endl;
		_tmpAgentGoals.push_back(Util::Point(_posData.at(a).back().x, 0.0f,_posData.at(a).back().y ));
		_posData.at(a).clear();

	}
	_posData.clear();

	for ( int a = 0; a < _numAgt; a++)
	{
		if ( tempPosData.at(a).size() > 0)
		{
			_posData.push_back(tempPosData.at(a));
			// _AgentGoals.push_back(_tmpAgentGoals.at(a));
		}
	}

	// update the number of agents left in the data
	_numAgt = _posData.size();
}


void CompositeTechniqueEntropy::performEstimation()
{

	//std::cout << "number of agents in the simulation " << this->getEngineInterface()->getAgents().size() <<
		//		" number of agents from the data " << _posData.size() << std::endl;
	int numFrames = readNoisyData2(this->_realDataFileName); // posData will be full now
	selectGoodData(numFrames);

/*
	for (int a = 0; a < _posData.size(); a++)
	{
		std::cout << "agent: " << a << " has " << _posData.at(a).size() <<
								" and starts at frame " << _agentStartFrame.at(a) << std::endl;

		for (int t = 0; t < _posData.at(a).size(); t++)
		{
		std::cout << _posData.at(a).at(t).x << "," << _posData.at(a).at(t).y << " ";
		}
		std::cout << std::endl;

	}
*/
	for (int a = 0; a < _posData.size(); a++)
	{
		float distance = 0;
		for (int t = 0; t < _posData.at(a).size()-1; t++)
		{
		// std::cout << _posData.at(a).at(t).x << "," << _posData.at(a).at(t).y << " ";
			float d_x = _posData.at(a).at(t+1).x - _posData.at(a).at(t).x;
			float d_y = _posData.at(a).at(t+1).y - _posData.at(a).at(t).y;
			distance = distance + sqrtf((d_x*d_x) +	(d_y*d_y));
		}

		// std::cout << "Average velocity of agent: " << a << " is " << distance/_posData.at(a).size() << std::endl;
		// std::cout << "Average velocity of agent: " << a << " is " << distance/(_posData.at(a).size()*_timeStep) << std::endl;
	}

	std::set<SpatialDatabaseItemPtr> neighborList;
	std::cout << "The number of agents in the simulation is: " << _numAgt << std::endl;

	initData(_timeStep, sumSamp, _numAgt, _posData); // This is done already via other means.
	setDIM();

	Matrix Sigma = m_identity(X_DIM);
	Matrix xHat = zeros(X_DIM);


	Matrix z = zeros(Z_DIM);
	Matrix u = zeros(U_DIM);

	std::vector<Matrix> X(_numSamples);

	M = m_identity(M_DIM);

	m_initGuess(Sigma,xHat);

	std::cout << "Writing out some dimensions" << std::endl;
	std::cout << "M_DIM: " << M_DIM << " N_DIM: " << N_DIM << std::endl;



	//Initalize samples in the ensemble
	/*
	 * 			agent0_data		agent1_data
	 *
	 * 				p_x				p_x			...
	 * Sample0		p_z				p_z
	 * 				v_x				v_x
	 * 				v_z				v_z
	 *
	 *
	 * 	Sample1		p_x
	 * 				p_z
	 * 				v_x
	 * 				v_z				.
	 * 									.
	 * 				.						.
	 * 				.
	 * 				.
	 */

	/*
	 * Is really designed to perform a random sample of fHat
	 */
	for (size_t i = 0; i < _numSamples; ++i)
	{// The number of samples
		X[i] = zeros(X_DIM);
		for (int a = 0; a < _numAgt; a++) // for each agent
		{
			Matrix sampx  = Util::sampleGaussian(xHat.subMatrix(a*sx,0,sx,1), M);
			// Matrix sampx  = sampleGaussian(xHat.subMatrix(a*sx,0,a*sx,1), M);
			for (int z = 0; z < sx; z++)// for each data dimension p_x, p_z, v_x, v_z
			{
				X[i][a*sx+z] = sampx[z];
			}
		}
	}

	/*
	 * Now run the EM-algorithm to estimate X (the true state) from
	 * Z (the noisy input data)
	 */
	// std::vector<std::vector<Matrix> > Xs;
	size_t ses = _posData[0].size();
	// int ses = 4;
	for (size_t s = 1; s < ses; s++)
	{ //for each timestep we compute an X
		for (size_t a = 0; a < _numAgt; a++)
		{
			z[2*a+0] = _posData[a][s].x;
			z[2*a+1] = _posData[a][s].y;
		}
		curStep = s;
		if (s == 26)
			s = 26;
		std::cout << "Calling Kalman filter, timestep: " << s << std::endl;
		// std::cout << "M_DIM: " << M_DIM << " N_DIM: " << N_DIM << std::endl;
		// std::cout << "z: " << z << std::endl;
		/*
		 *
		 */
		// std::cout << "number of agents in the simulation " << this->getEngineInterface()->getAgents().size() <<
			//		" number of agents from the data " << _posData.size() << std::endl;
		Util::ensembleKalmanFilter(X, u, z,  M_DIM, this, N_DIM, this); ////
		// ensembleKalmanFilter(X, u, z,  M_DIM, [=](int v){return this->m_fHat;}, N_DIM, &CompositeTechniqueEntropy::h); ////
		// this->getEngineInterface()->getSpatialDatabase()->getItemsInRange(neighborList, -30.0f, 30, -30, 30, NULL);
		// std::cout << "agents left in database: " << neighborList.size() << std::endl;
		xHat = zeros(X_DIM);
		for (size_t i = 0; i < X.size(); ++i)
		{
			xHat += X[i] / X.size();
		}
		_xHats.push_back(xHat);
		Xs.push_back(X);
	}
	xHat = zeros(X_DIM);
	for (size_t i = 0; i < X.size(); ++i)
	{
		xHat += X[i] / X.size();
	}
	Sigma = zeros(X_DIM,X_DIM);
	for (size_t i = 0; i < X.size(); ++i)
	{
		Sigma += (X[i] - xHat)*~(X[i] - xHat) / X.size();
	}

	// std::cout << "Ensemble Kalman Filter size: " << xHat.numColumns()*xHat.numRows() <<  std::endl << ~xHat << std::endl;
	// std::cout << "Sigma: " << std::endl << ~Sigma << std::endl;
	// std::cout << "Length of a sample: " <<  ((Xs[0][0]).numRows() * (Xs[0][0]).numColumns())  << std::endl;
	// std::cout << "Xs[0][0]: " << ~(Xs[0][0]) << std::endl;
	// std::cout << "Xs[0][numsamples]: " << ~(Xs[0][_numSamples-1]) << std::endl;
	// int tsSize = ((Xs[0][0]).numRows() * (Xs[0][0]).numColumns());


	float pos_dist_agents = 0;
	float vel_diff_agents = 0;
	for ( size_t agent = 0; agent < _posData.size(); agent++)
	{
		float distance = 0;
		float distance2 = 0;
		float pos_dist = 0;
		float vel_diff = 0;
		for (size_t ts = 0; ts < _xHats.size()-1; ts++)
		{
			float x_v = _xHats[ts][4*agent+2]; //*_timeStep; // distance traveled in one frame
			float y_v = _xHats[ts][4*agent+3]; //*_timeStep;
			float x_d = _xHats[ts+1][4*agent] - _xHats[ts][4*agent];
			float y_d = _xHats[ts+1][4*agent+1] - _xHats[ts][4*agent+1];
			float pos_x_diff = fabs(_xHats[ts][4*agent] - _posData.at(agent).at(ts).x);
			float pos_z_diff = fabs(_xHats[ts][4*agent+1] - _posData.at(agent).at(ts).y);
			float pos_x_v = (_posData.at(agent).at(ts+1).x - _posData.at(agent).at(ts).x);
			float pos_z_v = (_posData.at(agent).at(ts+1).y - _posData.at(agent).at(ts).y);
			Util::Vector tmpVelDiff = Util::Vector(pos_x_v, 0, pos_z_v)/_timeStep;
			// std::cout << "Velocity calced from raw data " << tmpVelDiff << " length " << tmpVelDiff.length() <<
				// 	" or length " << Util::Vector(pos_x_v, 0, pos_z_v).length()/_timeStep <<  std::endl;

			distance = distance + (sqrtf((x_v*x_v)+(y_v*y_v))*_timeStep);
			distance2 = distance2 + (sqrtf((x_d*x_d)+(y_d*y_d)));
			pos_dist = pos_dist + (sqrtf((pos_x_diff*pos_x_diff)+(pos_z_diff*pos_z_diff)));
			vel_diff = vel_diff + (Util::Vector(x_v, 0, y_v)-tmpVelDiff).length();
			// vel_diff
		}
		// std::cout << "Average velocity of agent: " << agent << " is " << distance/(_posData.at(agent).size()*_timeStep) << std::endl;
		// std::cout << "Average position velocity of agent: " << agent << " is " << distance2/(_posData.at(agent).size()*_timeStep) << std::endl;
		// std::cout << "Average position difference from data: " << agent << " is " << pos_dist/_xHats.size() << std::endl;

		pos_dist_agents =  pos_dist_agents + pos_dist/_xHats.size();
		vel_diff_agents = vel_diff_agents + vel_diff/_xHats.size();


	}
	std::cout << "Average position difference from data: " << pos_dist_agents/_posData.size() << std::endl;
	std::cout << "Average velocity difference from data: " << vel_diff_agents/_posData.size() << std::endl;

	/*
	for ( int ts = 0; ts < _posData[0].size()-1; ts++)
	{
		std::cout << "Xs[" << ts << "][0]: " << ~(Xs[ts][_numSamples-2]) << std::endl;
		/*
		for (int samp = 0; samp < _numSamples ; samp++)
		{
			for ( int d = 0; d < tsSize; d++)
			{
				if ( std::isnan(Xs[ts][samp][d]) )
				{
					std::cout << "Some of the data is nan" << std::endl;
				}
			}
		}
	}*/


	/************************************************************
	* End of EM estimation
	************************************************************/

	/**
	 * Now to calculate difference
	 */

	Matrix zu = zeros(U_DIM);
	Matrix zm = zeros(M_DIM);
	Matrix diff;
	M = zeros(M_DIM,M_DIM);
	for (int e = 0; e < _numSamples; e++)
	{//for each sample in the ensemple
		std::vector<Matrix> fxs;
		// fxs.push_back(Xs[0][e]); // Not used bu one needs to be added into data
		for (int s = 0; s < ses-2; s++)
		{ //for each timestep
			fxs.push_back(this->m_fHat(Xs[s][e],zu,zm));
			for (int a = 0; a < _numAgt; a++)
			{ // for each agent in the timestep
				// std::cout << "subMatrix s=" << s << " a=" << a << " : " << ~(fxs.back()) << std::endl;
				/*
				 * Compare computed next step to all the steps in the ensamble
				 */
				// diff = (fxs.back().subMatrix(a*sx,0,sx,1) - Xs[s+1][e].subMatrix(a*sx,0,sx,1));
				diff = abs(fxs.back().subMatrix(a*sx,0,sx,1) - Xs[s+1][e].subMatrix(a*sx,0,sx,1));
				M = M + diff*~diff;
				/*
				for ( int m = 0; m < (M.numRows()*M.numColumns()); m++)
				{
						if ( std::isnan(M[m]) )
						{
							std::cout << "Got some nan in the comparison" << std::endl;
						}
				}*/
			}
		}
	}
	std::cout << "The last diff was " << std::endl << ~diff << std::endl << diff*~diff << std::endl;
	std::cout << "Varience of M.subMatrix(0,0,2,2) " << std::endl <<  ~(M.subMatrix(0,0,2,2)) << std::endl;
	M = M / ((float)_numSamples*_numAgt*(ses-1));

	Matrix m2 = M.subMatrix(0,0,2,2);
	/*
	for (int i = 0; i < 2; i++)
	{
		double r_ = r(1);
		std::cout << "adding r " << r_ << " to m2(i,i) " << m2(i,i) << std::endl;
		m2(i,i) = m2(i,i); // *_numSamples; //  + 0.2f;
	}*/
// 		std::cout << "\n\n M2:\n " << m2 << "\n\n";
	m2 = m2 *_numSamples;
	std::cout << "Varience of m2 " << std::endl <<  ~(m2) << std::endl;

	double realDet = fabs(m2(0,0)*m2(1,1)-m2(1,0)*m2(0,1));//determinant
// 		double realDet = fabs(m2(0,0))+fabs(m2(1,1));//trace
// 		std::cout << "realDet: " << realDet << "\n\n";



	std::cout << "the determinate is " << realDet <<  std::endl;

	// realDet = fabs(realDet);
	double result = 0.5*log(pow(2*3.14159265*2.718281828,2)*realDet)/log(2.0);
	_entropyResult = result;

	std::cout << "Entropy result: " << result << std::endl;
}

/*
 * Adjust the number of agents in the simulation to match the data
 */
void CompositeTechniqueEntropy::initData(double & timestep, int & sumSamp, int & numAgt, std::vector<std::vector<mPair>> posData)
{

	if ( numAgt > this->getEngineInterface()->getAgents().size())
	{
		// Need more agents
		for (int i = (this->getEngineInterface()->getAgents().size()-1); i < (numAgt-1); i++ )
		{
			// Create agent from first and second position
			createAgent(posData.at(i).at(0), posData.at(i).at(1), i);
		}
	}
	else
	{
		// have to many agents
		for (int i = (this->getEngineInterface()->getAgents().size()-1); i >= numAgt; i--)
		{
			std::cout << "removing agent" << i << std::endl;
			this->removeAgent(this->getEngineInterface()->getAgents().at(i));
		}
	}

	/*
	 * Now compute a hueristic estimate of the agents final goal
	 * This adds a little space so at the end of the simulation the agent is
	 * not always "reaching its goal"
	 *
	 * Best idea add a vector to the final position that is the accumulation of
	 * the last second of data
	 */
	_AgentGoals.clear();
	unsigned int posSize = posData.at(0).size();
	for (int agent = 0; agent < posData.size(); agent++)
	{
		// std::cout << "Goal before: " << Util::Point(posData.at(agent).back().x, 0,
			//	posData.at(agent).back().y) << std::endl;
		posSize = posData.at(agent).size();
		Util::Vector goalAdd(0,0,0);
		for (int time = (1/_timeStep); time > 0; time--)
		{
			Util::Vector tmpAdd(0,0,0);
			tmpAdd.x = ((posData.at(agent).at(posSize-time)).x-
					(posData.at(agent).at((posSize-time)-1)).x);
			tmpAdd.z = ((posData.at(agent).at(posSize-time)).y -
					(posData.at(agent).at((posSize-time)-1)).y);

			goalAdd = goalAdd + tmpAdd;
		}

		// std::cout << "Calculated goalAdd: " << goalAdd << std::endl;
		_AgentGoals.push_back(Util::Point(posData.at(agent).back().x, 0,
				posData.at(agent).back().y)+goalAdd);
		// std::cout << "Goal after: " << _AgentGoals.back() << std::endl;
	}
}

SteerLib::AgentInitialConditions CompositeTechniqueEntropy::getAgentConditions(SteerLib::AgentInterface * agentInterface)
{
	SteerLib::AgentInitialConditions initialConditions;
	initialConditions.speed = 0;
	initialConditions.direction = agentInterface->forward();
	std::vector<AgentGoalInfo> goals;
	goals.push_back(agentInterface->currentGoal());
	initialConditions.goals = goals;
	initialConditions.radius = agentInterface->radius();
	initialConditions.name = "agent0";
	initialConditions.position = agentInterface->position();
	//initialConditions.
	return initialConditions;
}

/*
 * This method is just supposed to simulate a timestep. It does not need any frame information
 * The EKne will provided modified position data for each iteration as it works to approximate
 * the additional information from the noisy data
 */
Matrix CompositeTechniqueEntropy::m_fHat(const Matrix& x, const Matrix& u, const Matrix& m1)
{
	// std::cout << this->getEngineInterface() << std::endl;
	// std::cout << "agents ready " << this->getEngineInterface()->getAgents().size() << std::endl;


	SteerLib::AgentInterface * tempAgent;
	Util::Vector tempForward;
	// std::vector<SteerLib::AgentInterface>
	// for (int i = 0; i < m_numAgt; i++)
	float x_d = 0, y_d = 0, x_v = 0, y_v = 0;
	for (int a = 0; a < _numAgt; a++)
	{

		x_d = x[4*a]*_inverseScale;
		y_d = x[4*a+1]*_inverseScale;
		x_v = x[4*a+2]*_inverseScale;
		y_v = x[4*a+3]*_inverseScale;

		tempAgent = this->getEngineInterface()->getAgents().at(a);
		SteerLib::AgentInitialConditions initialConditions; // = this->getAgentConditions(tempAgent);
		initialConditions.radius = 0.50f;
		// initialConditions.
		initialConditions.position = Util::Point(x_d, 0.0f, y_d);
		initialConditions.speed = sqrt(((x_v*x_v ) + (y_v*y_v))); // /this->_timeStep;// basic length
		initialConditions.direction = normalize(Util::Vector(x_v, 0.0f, y_v));
		SteerLib::AgentGoalInfo agentGoal;
		agentGoal.goalType = SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET;
		agentGoal.targetIsRandom = false; // TODO This turned out to be extreamly important
		/*
		 * What if the agent is going in the other direction?
		 */
		// agentGoal.targetLocation = Util::Point(this->_posData[a].back().x*_inverseScale, 0.0f,
			// 	(this->_posData[a].back().y-1)*_inverseScale); // -1 to give the agent some more room to move near the end.
		// agentGoal.targetLocation = Util::Point(0,0,-10.0);// _AgentGoals.at(a);
		agentGoal.targetLocation = _AgentGoals.at(a)*_inverseScale;
		agentGoal.desiredSpeed = initialConditions.speed;
		agentGoal.targetDirection = normalize(agentGoal.targetLocation - initialConditions.position);

		initialConditions.goals.push_back(agentGoal);
		initialConditions.name = "agent" + std::to_string(a);
		/*
		initialConditions.position = Util::Point(x[4*a], 0.0f, x[4*a+1]);
		initialConditions.speed = sqrt(((x[4*a+2]*x[4*a+2] ) + (x[4*a+3]*x[4*a+3])); // /this->_timeStep;// basic length
		initialConditions.direction = normalize(Util::Vector(x[4*a+2], 0.0f, x[4*a+3]));
		SteerLib::AgentGoalInfo agentGoal;
		agentGoal.goalType = SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET;
		agentGoal.targetIsRandom = false; // TODO This turned out to be extreamly important
		agentGoal.targetLocation = Util::Point(this->_posData[a].back().x, 0.0f,
				this->_posData[a].back().y-1); // -1 to give the agent some more room to move near the end.
		// agentGoal.targetLocation = Util::Point(0,0,-10.0);// _AgentGoals.at(a);
		agentGoal.desiredSpeed = initialConditions.speed;
		agentGoal.targetDirection = normalize(agentGoal.targetLocation - initialConditions.position);

		initialConditions.goals.push_back(agentGoal);
		initialConditions.name = "agent" + std::to_string(a);
		*/
#ifdef _DEBUG_ENTROPY
		std::cout << "Speed given to agent " << tempAgent << " " << initialConditions.speed << " direction " <<
			initialConditions.direction << ", velocity x:" << x[4*a+2] << " z:" << x[4*a+3] << " and position " << initialConditions.position <<  std::endl;
#endif
		// SET_AGENT_i_POSITION_(x[4*i], x[4*i+1])
		// SET_AGENT_i_VELOCITY_(x[4*i], x[4*i+1])
		// SteerLib::AgentInterface * agent = this->getEngineInterface()->createAgent(initialconditions, moduleInterface);

		// this->getEngineInterface()->addAgent(agent, agentModule);
		// this->getEngineInterface()->getSpatialDatabase()->addObject()
		// this->createAgent( initialConditions);
		tempAgent->reset(initialConditions, this->getEngineInterface());
	}

	// std::cout << "agents ready " << this->getEngineInterface()->getAgents().size() << std::endl;

	// DO_SIMULATION_STEP
	/*
	 * I think I can accomplish this by just calling the update methods of
	 * each of the agents now.
	 */
	// I hope this works
	// std::cout << "Agent position before update " <<
		// 	this->getEngineInterface()->getAgents().at(0)->position() <<
			//		std::endl;
	// .......................................
	int timeCut = 1;
	this->_agentModule->preprocessFrame( 0 , _timeStep, 1);
	for (int i = 1; i <= timeCut; i++)
	{
		for (int i = 0; i < _numAgt; i++)
		{
			tempAgent = this->getEngineInterface()->getAgents().at(i);
			tempAgent->updateAI(i*(_timeStep/timeCut), _timeStep/timeCut, i );
		}
	}
	this->_agentModule->postprocessFrame( _timeStep , _timeStep, 1);

	/*
	for (int i = 0; i < _numAgt; i++)
	{
		tempAgent = this->getEngineInterface()->getAgents().at(i);
		tempAgent->updateAI(1.2f, _timeStep/2, 2 );
	}
	std::cout << std::endl;
	for (int i = 0; i < _numAgt; i++)
	{
		tempAgent = this->getEngineInterface()->getAgents().at(i);
		tempAgent->updateAI(1.2f, _timeStep/2, 3 );
	}*/

	// std::cout << "Agent position after update " <<
		// 	this->getEngineInterface()->getAgents().at(0)->position() <<
			// 		std::endl;


	Matrix xNew(4*_numAgt);
	double speedTrav = 0;
	for (int a = 0; a < _numAgt; a++){
		Matrix m = Util::sampleGaussian(zeros(4),m_identity(4));
		if (m1[0] == m1[1] && m1[1] == 0){
			m = zeros(4);
		}


		tempAgent = this->getEngineInterface()->getAgents().at(a);

		// std::cout << "Agent goal is " << tempAgent->currentGoal().targetLocation << std::endl;
		// speedTrav = ((tempAgent->position()-Util::Point(x[4*a], 0.0f, x[4*a+1])).length()/_timeStep);
		/*
		 * This rescalling to the previous vector size is important because
		 * agents will usually normalize their foward direction.
		 */
		// tempForward = tempAgent->forward() * ((sqrtf((x[4*a+2]*x[4*a+2] ) + (x[4*a+3]*x[4*a+3])))/_timeStep); // but this is old speed
		tempForward = tempAgent->velocity()/_inverseScale;
		// tempForward = tempAgent->velocity();
		// tempForward = tempAgent->forward() * ( speedTrav );
		// tempForward = (tempAgent->position()-Util::Point(x[4*a], 0.0f, x[4*a+1]));
		// tempForward = tempForward/_timeStep;
		// tempForward = tempAgent->forward() * tempForward.length();
		/*
		if ( std::isnan(tempAgent->position().x) || std::isnan(tempAgent->position().z))
		{
			std::cout << "The position was nan for some reason" << std::endl;
		}*/
		xNew[4*a] = (tempAgent->position().x/_inverseScale) + m[0];
		xNew[4*a+1] = (tempAgent->position().z/_inverseScale) + m[1];
		// xNew[4*a] = 0 + m[0];
		// xNew[4*a+1] = 0 + m[1];

		/*
		if ( ((tempAgent->position()-Util::Point(x[4*a], 0.0f, x[4*a+1])).length()/_timeStep) > 1.33f)
		{
			std::cout << "too fast " << ((tempAgent->position()-Util::Point(x[4*a], 0.0f, x[4*a+1])).length()/_timeStep) << std::endl;
		}*/
		/*
		if ( std::isnan(speedTrav)  )
		{
			std::cout << "The length traveled was nan for some reason" << std::endl;
		}
		else if ( std::isnan(tempForward.x) || std::isnan(tempForward.z) )
		{
			std::cout << "The part of the forward direction was nan for some reason" << std::endl;
		}*/
#ifdef _DEBUG_ENTROPY
		std::cout << "\tSpeed traveled " << tempAgent << "  after is " << speedTrav << " forward is " << tempAgent->forward() <<
			" position is " << tempAgent->position() << " and goal pos " << tempAgent->currentGoal().targetLocation << std::endl;
#endif
		// xNew[4*a+2] = 0 + m[2];
		// xNew[4*a+3] = 0 + m[3];
		xNew[4*a+2] = (tempForward.x) + m[2];
		xNew[4*a+3] = (tempForward.z) + m[3];
		// xNew[4*a+2] = 0; //(tempForward.x * 1) + m[2];
		// xNew[4*a+3] = -1.23;//  (tempForward.z * 1) + m[3];
		// xNew[4*a+2] = ( tempAgent->position().x-x[4*a] ) + m[2];
		// xNew[4*a+3] = ( tempAgent->position().z-x[4*a+1] ) + m[3];

	}

	_f_hat_calls = _f_hat_calls + 1;
	if ( _f_hat_calls > 25 )
	{
		// tempAgent = this->getEngineInterface()->getAgents().at(-1);
	}
#ifdef _DEBUG_ENTROPY
	std::cout << "*****************************Calls thus far " << _f_hat_calls << std::endl;
#endif
	/*
	for (int i = this->getEngineInterface()->getAgents().size(); i > 0; i--)
	{
		this->removeAgent(this->getEngineInterface()->getAgents().at(0));
	}
	this->_agentModule->cleanupSimulation();
	*/

	return xNew;
}

//Matrix CompositeTechniqueEntropy::m_fHatData(const Matrix& x, const Matrix& u, const Matrix& m1)
//{
	// MUBBASIR TODO -- I COMMENTED OUT THIS FUNCTION 
//}

void CompositeTechniqueEntropy::removeAgent(SteerLib::AgentInterface* agent)
{
	// this->getEngineInterface()->removeAgent(agent); // removed association in Engine interface
	if (agent->enabled()) // It is possable the agent will reach its goal and could be dissabled
	{
		agent->disable(); // diables and removes from spatial database
	}
	// This is the best method to deallocate an agent;
	this->getEngineInterface()->destroyAgent(agent);
	// this->_agentModule->destroyAgent(agent); // deallocates memory
}



SteerLib::AgentInterface * CompositeTechniqueEntropy::createAgent(SteerLib::AgentInitialConditions aIC)
{
	return this->getEngineInterface()->createAgent(aIC, _agentModule);
}

SteerLib::AgentInterface * CompositeTechniqueEntropy::createAgent(mPair mS, mPair mN, int i)
{
	SteerLib::AgentInitialConditions initialConditions; // = this->getAgentConditions(tempAgent);
	mPair diff;
	diff.x = mN.x - mS.x;
	diff.y = mN.y - mS.y;
	initialConditions.radius = 0.5f;

	// initialConditions.
	initialConditions.speed = sqrt((diff.x*diff.x ) + (diff.y*diff.y))*this->_timeStep;// basic length
	initialConditions.direction = Util::Vector(diff.x, 0.0f, diff.y);
	initialConditions.position = Util::Point(mS.x, 0.0f, mS.x);
	SteerLib::AgentGoalInfo agentGoal;
	agentGoal.goalType = SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET;
	agentGoal.targetLocation = Util::Point(mN.x, 0.0f, mN.y);
	agentGoal.targetIsRandom = false;
	initialConditions.goals.push_back(agentGoal);
	initialConditions.name = "agent" + std::to_string(i);
	// SET_AGENT_i_POSITION_(x[4*i], x[4*i+1])
	// SET_AGENT_i_VELOCITY_(x[4*i], x[4*i+1])
	// SteerLib::AgentInterface * agent = this->getEngineInterface()->createAgent(initialconditions, moduleInterface);

	// this->getEngineInterface()->addAgent(agent, agentModule);
	// this->getEngineInterface()->getSpatialDatabase()->addObject()
	// this->createAgent( initialConditions);

	return this->getEngineInterface()->createAgent(initialConditions, _agentModule);
}


void CompositeTechniqueEntropy::update(SteerLib::EngineInterface * engineInterface,
		float currentTimeStamp, float timePassedSinceLastFrame, unsigned int frameNumber)
{

	// std::cout << "currentTime: " << currentTimeStamp << ", dt: " << timePassedSinceLastFrame << ", frameNumber: " << frameNumber << std::endl;

	if ( !_estimationDone )
	{
		if ( _replay_data == 0)
		{
			this->performEstimation();
			this->_estimationDone = 1;
		}
		else
		{
			// In this case I want to run the real world data
			int numFrames = readNoisyData2(this->_realDataFileName);
			std::cout << "Number of agents in the simulation again " << _numAgt << std::endl;
			initData(_timeStep, sumSamp, _numAgt, _posData); // This is done already via other means.
			setDIM();
			this->_estimationDone = 1;

		}
	}

	 // std::cout << "Xs.size(): " << Xs.size() << " Xs[0].size(): " << Xs[0].size() <<
		//  	" Xs[0][0].size(): " << Xs[0][0].numRows()*Xs[0][0].numColumns() << std::endl;
	// std::cout << "Xs.size(): " << Xs.size() << " Xs[1].size(): " << Xs[1].size() << std::endl;
	// std::cout << "Xs.size(): " << Xs.size() << " Xs[1].size(): " << Xs[1].size() << std::endl;
				// " Xs[0][0].size(): " << Xs[1][0].numRows()*Xs[1][0].numColumns() << std::endl;

	for (size_t a = 0; a < _numAgt; a++)
	{
		// std::cout << "agentSpecificFrame: " << agentspecificFrame << std::endl;
		if ( (frameNumber ) <= (_agentStartFrame[a]))
		// if ( (frameNumber ) > (_agentStartFrame[a]))
		{
			// std::cout << "starting agent " << a << " fame start is " << (_agentStartFrame[a]) << " at frame " << frameNumber << std::endl;

			// std::cout << "agentSpecificFrameAgain: " << agentspecificFrame << std::endl;
			/*
			 * Use to replay raw real world data
			 */
			// Util::Point currentPoint = Util::Point(_posData[a].at(frameNumber).x*_inverseScale, 0, _posData[a].at(frameNumber).y*_inverseScale);
			// Util::Point nextPoint = Util::Point(_posData[a].at(frameNumber+1).x*_inverseScale, 0, _posData[a].at(frameNumber+1).y*_inverseScale);
			// Util::Vector velocity = nextPoint - currentPoint;

			/*
			 * Use to start agent at proper times to simulate steering for real
			 * world data.
			 */
			Util::Point currentPoint = Util::Point(_posData[a].at(0).x*_inverseScale, 0, _posData[a].at(0).y*_inverseScale);
			Util::Point nextPoint = Util::Point(_posData[a].at(1).x*_inverseScale, 0, _posData[a].at(1).y*_inverseScale);
			Util::Vector velocity = nextPoint - currentPoint;

			// Util::Point currentPoint = Util::Point(_xHats[frameNumber][4*a]*_inverseScale, 0, _xHats[frameNumber][4*a+1]*_inverseScale);
			// Util::Vector velocity = Util::Vector(_xHats[frameNumber][4*a+2]*_inverseScale, 0, _xHats[frameNumber][4*a+3]*_inverseScale);


			// this->getEngineInterface()->get
			// std::cout << "got data: " << agentspecificFrame << std::endl;
			SteerLib::AgentInitialConditions initialConditions; // = this->getAgentConditions(tempAgent);
			initialConditions.radius = 0.50f;
			initialConditions.speed = (velocity).length()/_timeStep;
			// initialConditions.speed = 1.33f;
			initialConditions.direction = normalize(velocity);
			initialConditions.position = currentPoint;
			SteerLib::AgentGoalInfo agentGoal;
			agentGoal.goalType = SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET;
			// agentGoal.targetLocation = Util::Point(this->_posData[a].back().x*_inverseScale, 0.0f,
				// 			(this->_posData[a].back().y-1)*_inverseScale); // -1 to give the agent some more room to move near the end.
			// agentGoal.targetLocation = Util::Point(this->_posData[a].back().x, 0.0f,
				//			this->_posData[a].back().y-1);
			// agentGoal.targetLocation = Util::Point(0,0,-10.0);// _AgentGoals.at(a);
			agentGoal.targetLocation = _AgentGoals.at(a)*_inverseScale;
			agentGoal.targetIsRandom = false;
			initialConditions.goals.push_back(agentGoal);
			initialConditions.name = "agent" + std::to_string(a);

			// Make sure the testcase included numAgt number of agents
			// std::cout << initialConditions.name << " ready at point: " <<
				//  	initialConditions.position << " with velocity " << velocity << std::endl;
			this->getEngineInterface()->getAgents().at(a)->reset(initialConditions, this->getEngineInterface());
			// std::cout << "got data: " << agentspecificFrame << std::endl;
		}

		else if ( (frameNumber - _agentStartFrame[a]) < _posData.at(a).size()-2 )
		{
			// std::cout << "starting agent " << a << " fame start is " << (_agentStartFrame[a]) << " at frame " << frameNumber << std::endl;

			//  Use to replay raw real world data
			// Util::Point currentPoint = Util::Point(_posData[a].at(frameNumber).x*_inverseScale, 0, _posData[a].at(frameNumber).y*_inverseScale);
			// Util::Point nextPoint = Util::Point(_posData[a].at(frameNumber+1).x*_inverseScale, 0, _posData[a].at(frameNumber+1).y*_inverseScale);
			// Util::Vector velocity = nextPoint - currentPoint;

			Util::Point currentPoint = Util::Point(_posData[a].at((frameNumber - _agentStartFrame[a])).x*_inverseScale, 0, _posData[a].at((frameNumber - _agentStartFrame[a])).y*_inverseScale);
			Util::Point nextPoint = Util::Point(_posData[a].at((frameNumber - _agentStartFrame[a])+1).x*_inverseScale, 0, _posData[a].at((frameNumber - _agentStartFrame[a])+1).y*_inverseScale);
			Util::Vector velocity = nextPoint - currentPoint;

			// Util::Point currentPoint = Util::Point(_xHats[frameNumber][4*a]*_inverseScale, 0, _xHats[frameNumber][4*a+1]*_inverseScale);
			// Util::Vector velocity = Util::Vector(_xHats[frameNumber][4*a+2]*_inverseScale, 0, _xHats[frameNumber][4*a+3]*_inverseScale);


			// this->getEngineInterface()->get
			// std::cout << "got data: " << agentspecificFrame << std::endl;
			SteerLib::AgentInitialConditions initialConditions; // = this->getAgentConditions(tempAgent);
			initialConditions.radius = 0.50f;
			initialConditions.speed = (velocity).length()/_timeStep;
			// initialConditions.speed = 1.33f;
			initialConditions.direction = normalize(velocity);
			initialConditions.position = currentPoint;
			SteerLib::AgentGoalInfo agentGoal;
			agentGoal.goalType = SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET;
			// agentGoal.targetLocation = Util::Point(this->_posData[a].back().x*_inverseScale, 0.0f,
				// 			(this->_posData[a].back().y-1)*_inverseScale); // -1 to give the agent some more room to move near the end.
			// agentGoal.targetLocation = Util::Point(this->_posData[a].back().x, 0.0f,
				//			this->_posData[a].back().y-1);
			// agentGoal.targetLocation = Util::Point(0,0,-10.0);// _AgentGoals.at(a);
			agentGoal.targetLocation = _AgentGoals.at(a)*_inverseScale;
			agentGoal.targetIsRandom = false;
			initialConditions.goals.push_back(agentGoal);
			initialConditions.name = "agent" + std::to_string(a);

			// Make sure the testcase included numAgt number of agents
			// std::cout << initialConditions.name << " ready at point: " <<
				//  	initialConditions.position << " with velocity " << velocity << std::endl;
			this->getEngineInterface()->getAgents().at(a)->reset(initialConditions, this->getEngineInterface());
			// std::cout << "got data: " << agentspecificFrame << std::endl;
		}
		else
		{
			if (this->getEngineInterface()->getAgents().at(a)->enabled())
			{
				// std::cout << "disabling agent " << a << std::endl;
				// this->getEngineInterface()->getAgents().at(a)->disable();
			}
		}
	}
	/*
	 *
	SteerLib::AgentInitialConditions initialConditions;
	Util::Point testP(2, 0,  1);
	initialConditions.speed = 0;
	initialConditions.direction = engineInterface->getAgents().at(0)->forward();
	std::vector<AgentGoalInfo> goals;
	goals.push_back(engineInterface->getAgents().at(0)->currentGoal());
	initialConditions.goals = goals;
	initialConditions.radius = engineInterface->getAgents().at(0)->radius();
	initialConditions.name = "agent0";
	initialConditions.position = testP;
	engineInterface->getAgents().at(0)->reset(initialConditions, engineInterface);
	*/

}



void CompositeTechniqueEntropy::update(SimulationMetricsCollector * simulationMetrics, float timeStamp, float dt)
{
	/*
	if ( !_estimationDone )
	{
		this->performEstimation();
		this->_estimationDone = 1;
	}*/
	// simulationMetrics->getAgentCollector(0)->
	std::cout << "We can get what we want here!!, engineInterface: " << this->getEngineInterface() << std::endl;
	// const std::vector<SteerLib::AgentInterface*> & agents = this->getEngineInterface()->getClock();
	//const SteerLib::Clock & clock = this->getEngineInterface()->getClock();


	// std::cout << "The time is: " << this->getEngineInterface()->getClock().getCurrentSimulationTime() << std::endl;
	// if (((unsigned int) agents) != NULL)
	{
		//for (int i=0; i < agents.size(); i++)
		{
			// std::cout << "Agent " << 0 << ": "<< agents[0]->position() << std::endl;
		}
	}

}
/*
void update(SteerLib::SpatialDataBaseInterface * gridDB, const std::vector<SteerLib::AgentInterface*> & updatedAgents, float currentTimeStamp, float timePassedSinceLastFrame)
{
	std::cout << "We can get what we want here!! too" << std::endl;
}
*/
float CompositeTechniqueEntropy::getTotalBenchmarkScore(SimulationMetricsCollector * simulationMetrics)
{
	return _entropyResult;
}

float CompositeTechniqueEntropy::getAgentBenchmarkScore(unsigned int agentIndex, SimulationMetricsCollector * simulationMetrics) {
	return 0;
}

void CompositeTechniqueEntropy::printTotalScoreDetails(SimulationMetricsCollector * simulationMetrics, std::ostream & out) {



}

void CompositeTechniqueEntropy::printAgentScoreDetails(unsigned int agentIndex, SimulationMetricsCollector * simulationMetrics, std::ostream & out) {


}


float CompositeTechniqueEntropy::_computeTotalBenchmarkScore(SimulationMetricsCollector * simulationMetrics)
{
	return 0;
}
