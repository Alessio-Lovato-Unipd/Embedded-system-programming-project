// Internal dependencies
#include "rover/publisher.h"

using namespace std::chrono_literals;

Rover::Rover(Gestore_robot &server, int id, const dati_robot &posizione_robot, Mappa &mappa_riferimento)
      : server_{server}, id_(id), mappa_{mappa_riferimento},
	    robot_ {server.crea_robot(posizione_robot.posizione_robot(), mappa_riferimento, posizione_robot.dimensione())}
{
	timer_ = g_node->create_wall_timer(500ms, std::bind(&Rover::timer_callback, this));
}
void Rover::operator()() {
	bool ho_ancora_obbiettivi{true};
	while (ho_ancora_obbiettivi || server_.satelliti_con_dati_presenti()) {
		posizione nuovo_obbiettivo{server_.ottieni_prossimo_obbiettivo(robot_.posizione_centrale())};
		server_.assegna_obbiettivo(robot_, nuovo_obbiettivo);
		std::osyncstream robot_a_cout(std::cout);
		robot_a_cout << "Robot " << std::to_string(id_) << " nuovo obbiettivo: {" << robot_.obbiettivo().first <<
		"; " << robot_.obbiettivo(). second << "}" << endl;
		if (!server_.obbiettivi_presenti())
			ho_ancora_obbiettivi = false;
	}
	std::osyncstream robot_a_cout(std::cout);
	robot_a_cout << "robot " << std::to_string(id_) << " ha finito i task" << endl;
}

void Rover::timer_callback() {
	while (!robot_.obbiettivo_raggiunto()) {
		server_.sposta_robot(robot_);
		std::osyncstream robot_a_cout(std::cout);
		robot_a_cout << "Robot " << std::to_string(id_) << " : {" << robot_.posizione_centrale().first << ", "
				<< robot_.posizione_centrale().second << "}" << endl;
	}

  auto message{rover_visualizer::msg::RoverPosition()};

  message.id = id_;
  message.position.x = robot_.posizione_centrale().first;
  message.position.y = robot_.posizione_centrale().second;

  g_publisher->publish(message);
}

