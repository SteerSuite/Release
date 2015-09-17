//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/*
 * CompositeTechniqueEntropy.h
 *
 *  Created on: 2013-09-22
 *      Author: glenpb
 */

#ifndef COMPOSITETECHNIQUEENTROPY_H_
#define COMPOSITETECHNIQUEENTROPY_H_

#include "Globals.h"
#include "interfaces/EngineInterface.h"
#include "interfaces/BenchmarkTechniqueInterface.h"
#include "util/GenericException.h"
#include "interfaces/AgentInterface.h"
#include "util/dmatrix.h"
#include "CompositeTechnique02.h"

//  rm frames/frame*.ppm; ../build/bin/steersim -module scenario,scenarioAI=pprAI,useBenchmark,benchmarkTechnique=compositeEntropy,benchmarkLog=data//0/test.log,checkAgentValid,reducedGoals,fixedSpeed,checkAgentRelevant,minAgents=3,ailogFileName=data//0/pprAI.log,maxFrames=2000,checkAgentInteraction,egocentric,RealDataName=data/RealWorldData/bot-300-050-050_combined_MB.txt,scenarioSetPath=data/RealWorldData/ou-060-180-180/,scenarioSetInitId=0,numScenarios=1,dbName=steersuitedb,skipInsert=True,ped_max_speed=4.000000,ped_max_force=11.477351,ped_max_speed_factor=1.036524,ped_faster_speed_factor=1.910091,ped_slightly_faster_speed_factor=3.193959,ped_typical_speed_factor=1.500000,ped_slightly_slower_speed_factor=0.700720,ped_slower_speed_factor=0.633337,ped_cornering_turn_rate=3.760000,ped_adjustment_turn_rate=1.540000,ped_faster_avoidance_turn_rate=1.154095,ped_typical_avoidance_turn_rate=0.182240,ped_braking_rate=0.661048,ped_comfort_zone=0.753389,ped_query_radius=7.348530,ped_similar_direction_dot_product_threshold=0.856956,ped_same_direction_dot_product_threshold=0.890000,ped_oncoming_prediction_threshold=-0.924088,ped_oncoming_reaction_threshold=-0.869304,ped_wrong_direction_dot_product_threshold=0.580059,ped_threat_distance_threshold=12.600148,ped_threat_min_time_threshold=1.166572,ped_threat_max_time_threshold=4.975509,ped_predictive_anticipation_factor=5.844836,ped_reactive_anticipation_factor=0.330000,ped_crowd_influence_factor=0.247909,ped_facing_static_object_threshold=0.215440,ped_ordinary_steering_strength=0.050671,ped_oncoming_threat_avoidance_strength=0.075143,ped_cross_threat_avoidance_strength=0.874220,ped_max_turning_rate=0.230000,ped_feeling_crowded_threshold=4.000000,ped_scoot_rate=0.371777,ped_reached_target_distance_threshold=0.614635,ped_dynamic_collision_padding=0.305582,ped_furthest_local_target_distance=36.000000,ped_next_waypoint_distance=38.000000,ped_max_num_waypoints=16.000000,recFile=ppr_opt_18-agent.rec -config configs/Entropy-config.xml -saveFramesTo frames/

namespace SteerLib
{

	class STEERLIB_API CompositeTechniqueEntropy : public SteerLib::CompositeBenchmarkTechnique02
	{
	public:
		CompositeTechniqueEntropy();
		virtual ~CompositeTechniqueEntropy();

		// void CompositeTechniqueEntropy::preprocessFrame(float currentSimulationTime, float simulatonDt, unsigned int currentFrameNumber);

		void init();
		void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo );
		void update(SimulationMetricsCollector * simulationMetrics, float timeStamp, float dt);
		void update(SteerLib::EngineInterface * engineInterface, float currentTimeStamp, float timePassedSinceLastFrame, unsigned int frameNumber);

		SteerLib::AgentInitialConditions getAgentConditions(SteerLib::AgentInterface * interface_);

		void performEstimation();

		void readNoisyData(std::string file);
		int readNoisyData2(std::string file);

		void selectGoodData(unsigned int x);
		// void update(SteerLib::SpatialDataBaseInterface * gridDB, const std::vector<SteerLib::AgentInt erface*> & updatedAgents, float currentTimeStamp, float timePassedSinceLastFrame);
		void reset ()
		{
			_benchmarkScoreComputed = false;
			_numCollisionsOfAllAgents = 0;
			_numAgents = 0.0f;
			_totalTimeOfAllAgents = 0.0f;
			_totalEnergyOfAllAgents = 0.0f;
			_totalBenchmarkScore = 0.0f;
		}

		virtual void getScores ( std::vector<float> & scores )
		{
			scores.push_back(_numCollisionsOfAllAgents);
			scores.push_back(_totalTimeOfAllAgents);
			scores.push_back(_totalEnergyOfAllAgents);
			scores.push_back(_totalInstantaneousAcceleration);
		}
		void getAgentScores ( unsigned int agentIndex, SimulationMetricsCollector * simulationMetrics,std::vector<float> & scores )
		{
			CompositeBenchmarkTechnique02::getAgentScores(agentIndex, simulationMetrics, scores);
			// TODO std::cerr << "need to implement \n";
		}


		float getTotalBenchmarkScore(SimulationMetricsCollector * simulationMetrics);
		float getAgentBenchmarkScore(unsigned int agentIndex, SimulationMetricsCollector * simulationMetrics);
		void printTotalScoreDetails(SimulationMetricsCollector * simulationMetrics, std::ostream & out);
		void printAgentScoreDetails(unsigned int agentIndex, SimulationMetricsCollector * simulationMetrics, std::ostream & out);

		void removeAgent(SteerLib::AgentInterface* a);
		SteerLib::AgentInterface * createAgent(SteerLib::AgentInitialConditions f);
		SteerLib::AgentInterface * createAgent(mPair mS, mPair mN, int i);
		SteerLib::ModuleInterface* _agentModule;


		/*
		 * simulators
		 */
		// real simulator
		Matrix m_fHat(const Matrix& x, const Matrix& u, const Matrix& m1);
		// just compairs real data to estimated data
		Matrix m_fHatData(const Matrix& x, const Matrix& u, const Matrix& m1);
		// measurement update
		Matrix h(const Matrix & x, const Matrix & n);

	protected:
		float _computeTotalBenchmarkScore(SimulationMetricsCollector * simulationMetrics);

		bool _benchmarkScoreComputed;
		float _alpha, _beta, _gamma;
		float _numCollisionsOfAllAgents, _totalTimeOfAllAgents, _totalEnergyOfAllAgents;
		float _numAgents;
		float _totalBenchmarkScore;

		int sx;
		int sumSamp ;
		int curStep ;
		double _timeStep;
		int _numAgt;
		Matrix _xHat;
		int _numSamples;
		float _invNumSamples;
		double _entropyResult;

		unsigned int _perferedNumAgents;
		unsigned int _perferedNumFrames;

		/*
		 * This will be filled with a number of samples
		 * Matrix0
		 * 						frame0				frame1				.	.	.
		 * 			agent0	p_x, p_z, v_x, v_z	p_x, p_z, v_x, v_z   	.	.	.
		 * 			agent1	p_x, p_z, v_x, v_z
		 * 			.				.			.
		 * 			.				.				.
		 * 			.				.					.
		 *
		 * Matrix1
		 * 						frame0				frame1				.	.	.
		 * 			agent0	p_x, p_z, v_x, v_z	p_x, p_z, v_x, v_z   	.	.	.
		 * 			agent1	p_x, p_z, v_x, v_z
		 * 			.				.			.
		 * 			.				.				.
		 * 			.				.					.
		 */
		std::vector<std::vector<Matrix> > Xs;

		/*
		 * This will contain the estimated data
		 *
		 * frame0 a0p_x, a0p_z, a0v_x, a0v_z	a1p_x, a1p_z, a1v_x, a1v_z   	.	.	.
		 * frame1 a0p_x, a0p_z, a0v_x, a0v_z	a1p_x, a1p_z, a1v_x, a1v_z   	.	.	.
		 * .
		 * .
		 * .
		 */
		std::vector<Matrix> _xHats;

		std::string _realDataFileName;
		std::string _agentModuleName;
		float _realWorldDataScale;
		/*
		 *  This is to map the data back to the scale of the environment.
		 *  Mostly because some bevaviors depened on the radius of the agent
		 */
		float _inverseScale;
		std::vector<unsigned int> _agentStartFrame;

		double r(double scale = 1)
		{
		  return scale*((2.f*rand()/RAND_MAX)-1);
		}

		#define MAX_AGENTS 132

		/*			ts_1	ts_2
		 * agent0	p_x,p_y	p_x,p_y
		 * agent1	p_x,p_y	p_x,p_y
		 */
		std::vector<std::vector<mPair>> _posData;
		std::vector<Util::Point> _AgentGoals;

		int X_DIM;  // State dimension
		int U_DIM;  // Control Input dimension
		int Z_DIM; // Measurement dimension
		int M_DIM;  // Motion Noise dimension
		int N_DIM;  // Measurement Noise dimension
		Matrix M;

		void setDIM()
		{
			X_DIM=sx*_numAgt;  // State dimension
			U_DIM=2;  // Control Input dimension
			Z_DIM=2*_numAgt; // Measurement dimension
			M_DIM=sx;  // Motion Noise dimension
			N_DIM=2*_numAgt;  // Measurement Noise dimension
		}

		/*******************************************************************
		 * Stuff for the Entropy metric to simulate the scenario for its EM-algorithm
		 */
		double vPref;

		unsigned int _estimationDone;

		void m_initGuess(Matrix &Sigma, Matrix &xHat);


		void initData(double & timestep, int & sumSamp, int & numAgt, std::vector<std::vector<mPair>> posData);

		/*
		 * This area is mostly for debugging
		 */
		unsigned int _f_hat_calls;
		unsigned int _replay_data;

	};
}

#endif /* COMPOSITETECHNIQUEENTROPY_H_ */
