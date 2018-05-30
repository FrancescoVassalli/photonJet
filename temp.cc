for (int iEvent = 0; iEvent < 1; ++iEvent) //WARNING! WARNING! WARNING!: This takes 2.5 hours at 1 Million events//
    {
      p = 0;
      if (!pythia.next()) continue;

      for(int j = 0; j < 20; j++)
      {
        if(pythia.event[j].id() == 22 and pythia.event[j].pT() >= 10)
        {
          std::cout<<"_______________________________________________________________________________________________________________________"<<std::endl;
          std::cout<<"||                                   Events that contain isophoton                                                   ||"<<std::endl;
          std::cout<<"-----------------------------------------------------------------------------------------------------------------------"<<std::endl;
          std::cout<<"|| Particle  |  Status   |   ID   |   pT  |   eta   |   phi   |  mother 1  |  mother 2  |  daughter 1  |  daughter 2 ||"<<std::endl;
          std::cout<<"-----------------------------------------------------------------------------------------------------------------------"<<std::endl;
          for(int i = 0; i < pythia.event.size(); i++)
          { 

            std::cout<<"||   "<<i<<"    |    "<<pythia.event[i].status()<<"    |    "<<pythia.event[i].id()<<"    |    "<<pythia.event[i].pT()<<"    |    "<< pythia.event[i].eta()<<"    |    "
            <<pythia.event[i].phi()<<"    |    "<< pythia.event[i].mother1()<<"    |    "<< pythia.event[i].mother2()<<"    |    "<<pythia.event[i].daughter1()<<"    |    "<<pythia.event[i].daughter2()<<"    |    "<<std::endl;
          }
          std::cout<<"||_____________________________________________________________________________________________________________________" <<std::endl;
          std::cout<<std::endl;
          std::cout<<std::endl;
          std::cout<<std::endl;

        t->Fill(); //writing data collected to the tree
        break;
        }
      }
	}