/**
 * This is an auto-generated file by Optimization Engine (OpEn)
 * OpEn is a free open-source software - see doc.optimization-engine.xyz
 * dually licensed under the MIT and Apache v2 licences.
 *
 */
#include "ros/ros.h"
#include "nmpc_blimp_formation_planner_msgs/OptimizationResult.h"
#include "nmpc_blimp_formation_planner_msgs/OptimizationParameters.h"
#include "nmpc_blimp_formation_optimizer_bindings.hpp"
#include "open_optimizer.hpp"

namespace nmpc_blimp_formation_planner {
/**
 * Class nmpc_blimp_formation_planner::OptimizationEngineManager manages the
 * exchange of data between the input and output topics
 * of this node
 */
class OptimizationEngineManager {
/**
 * Private fields and methods
 */
private:
    /**
     * Optimization parameters announced on the corresponding
     * topic (nmpc_blimp_formation_planner/parameters)
     */
    nmpc_blimp_formation_planner_msgs::OptimizationParameters params;
    /**
     * Object containing the result (solution and solver
     * statistics), which will be announced on nmpc_blimp_formation_planner/results
     */
    nmpc_blimp_formation_planner_msgs::OptimizationResult results;
    /**
     * Vector of parameters (provided by the client on
     * nmpc_blimp_formation_planner/parameters)
     */
    double p[NMPC_BLIMP_FORMATION_OPTIMIZER_NUM_PARAMETERS] = { 0 };
    /**
     * Solution vector
     */
    double u[NMPC_BLIMP_FORMATION_OPTIMIZER_NUM_DECISION_VARIABLES] = { 0 };
    /**
     * Vector of Lagrange multipliers (if any)
     */
    double *y = NULL;
    /**
     * Workspace variable used by the solver - initialised once
     */
    nmpc_blimp_formation_optimizerCache* cache;
    /**
     * Initial guess for the penalty parameter
     */
    double init_penalty = ROS_NODE_NMPC_BLIMP_FORMATION_OPTIMIZER_DEFAULT_INITIAL_PENALTY;

    /**
     * Publish obtained results to output topic
     */
    void publishToTopic(ros::Publisher& publisher)
    {
        publisher.publish(results);
    }

    /**
     * Updates the input data based on the data that are posted
     * on /mpc/open_parameters (copies value from topic data to
     * local variables). This method is responsible for parsing
     * the data announced on the input topic.
     */
    void updateInputData()
    {
        init_penalty = (params.initial_penalty > 1.0)
            ? params.initial_penalty
            : ROS_NODE_NMPC_BLIMP_FORMATION_OPTIMIZER_DEFAULT_INITIAL_PENALTY;

        if (params.parameter.size() > 0) {
            for (size_t i = 0; i < NMPC_BLIMP_FORMATION_OPTIMIZER_NUM_PARAMETERS; ++i)
                p[i] = params.parameter[i];
        }

        if (params.initial_guess.size() == NMPC_BLIMP_FORMATION_OPTIMIZER_NUM_DECISION_VARIABLES) {
            for (size_t i = 0; i < NMPC_BLIMP_FORMATION_OPTIMIZER_NUM_DECISION_VARIABLES; ++i)
                u[i] = params.initial_guess[i];
        }

		if (params.initial_y.size() == NMPC_BLIMP_FORMATION_OPTIMIZER_N1) {
            for (size_t i = 0; i < NMPC_BLIMP_FORMATION_OPTIMIZER_N1; ++i)
                y[i] = params.initial_y[i];
		}
    }

    /**
     * Call OpEn to solve the problem
     */
    nmpc_blimp_formation_optimizerSolverStatus solve()
    {
        return nmpc_blimp_formation_optimizer_solve(cache, u, p, y, &init_penalty);
    }
/**
 * Public fields and methods
 */
public:
    /**
     * Constructor of OptimizationEngineManager
     */
    OptimizationEngineManager()
    {
	    y = new double[NMPC_BLIMP_FORMATION_OPTIMIZER_N1];
        cache = nmpc_blimp_formation_optimizer_new();
    }

    /**
     * Destructor of OptimizationEngineManager
     */
    ~OptimizationEngineManager()
    {
		if (y!=NULL) delete[] y;
        nmpc_blimp_formation_optimizer_free(cache);
    }

    /**
     * Copies results from `status` to the local field `results`
     */
    void updateResults(nmpc_blimp_formation_optimizerSolverStatus& status)
    {
        std::vector<double> sol(u, u + NMPC_BLIMP_FORMATION_OPTIMIZER_NUM_DECISION_VARIABLES);
        results.solution = sol;
        std::vector<double> y(status.lagrange, status.lagrange + NMPC_BLIMP_FORMATION_OPTIMIZER_N1);
        results.lagrange_multipliers = y;
        results.inner_iterations = status.num_inner_iterations;
        results.outer_iterations = status.num_outer_iterations;
        results.norm_fpr = status.last_problem_norm_fpr;
        results.cost = status.cost;
        results.penalty = status.penalty;
        results.status = (int)status.exit_status;
        results.solve_time_ms = (double)status.solve_time_ns / 1000000.0;
        results.infeasibility_f2 = status.f2_norm;
        results.infeasibility_f1 = status.delta_y_norm_over_c;
    }

    /**
     * Callback that obtains data from topic `/nmpc_blimp_formation_planner/open_params`
     */
    void mpcReceiveRequestCallback(
        const nmpc_blimp_formation_planner_msgs::OptimizationParameters::ConstPtr& msg)
    {
        params = *msg;
    }

    void solveAndPublish(ros::Publisher& publisher)
    {
        updateInputData(); /* get input data */
        nmpc_blimp_formation_optimizerSolverStatus status = solve(); /* solve!  */
        updateResults(status); /* pack results into `results` */
        publishToTopic(publisher);
    }
}; /* end of class OptimizationEngineManager */
} /* end of namespace nmpc_blimp_formation_planner */

/**
 * Main method
 *
 * This advertises a new (private) topic to which the optimizer
 * announces its solution and solution status and details. The
 * publisher topic is 'nmpc_blimp_formation_planner/result'.
 *
 * It obtains inputs from 'nmpc_blimp_formation_planner/parameters'.
 *
 */
int main(int argc, char** argv)
{
    std::string result_topic, params_topic;  /* parameter and result topics */
    double rate; /* rate of node (specified by parameter) */

    nmpc_blimp_formation_planner::OptimizationEngineManager mng;
    ros::init(argc, argv, ROS_NODE_NMPC_BLIMP_FORMATION_OPTIMIZER_NODE_NAME);
    ros::NodeHandle private_nh("~");

    /* obtain parameters from config/open_params.yaml file */
    private_nh.param("result_topic", result_topic,
                     std::string(ROS_NODE_NMPC_BLIMP_FORMATION_OPTIMIZER_RESULT_TOPIC));
    private_nh.param("params_topic", params_topic,
                     std::string(ROS_NODE_NMPC_BLIMP_FORMATION_OPTIMIZER_PARAMS_TOPIC));
    private_nh.param("rate", rate,
                     double(ROS_NODE_NMPC_BLIMP_FORMATION_OPTIMIZER_RATE));

    ros::Publisher mpc_pub
        = private_nh.advertise<nmpc_blimp_formation_planner_msgs::OptimizationResult>(
            result_topic,
            ROS_NODE_NMPC_BLIMP_FORMATION_OPTIMIZER_RESULT_TOPIC_QUEUE_SIZE);
    ros::Subscriber sub
        = private_nh.subscribe(
            params_topic,
            ROS_NODE_NMPC_BLIMP_FORMATION_OPTIMIZER_PARAMS_TOPIC_QUEUE_SIZE,
            &nmpc_blimp_formation_planner::OptimizationEngineManager::mpcReceiveRequestCallback,
            &mng);
    ros::Rate loop_rate(rate);

    while (ros::ok()) {
        mng.solveAndPublish(mpc_pub);
        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}