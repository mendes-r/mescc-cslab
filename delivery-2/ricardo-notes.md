# NotesG

### Respostas-1

24-01-04

- Apenas uma bomba por WPS, contudo podemos tornar o systema mais complexo... se chegar a um nível médio trigger second pump... se uma bomba falhar, o nivel médio passa a ser o nível máximo.

- Quando o nível máximo é alcançado um alarme soa nos RSS. O butão apenas serve para calar o alarme. Nenhuma ação é necessária quando o alarme é acionado (nada de fechar inputs ou coisas semelhantes)

- Um RSS mostra os estado de todos os WSP

- No final de CSLAB podemos simular apenas um dos WSP, contudo temos de mostrar que é possível adicionar mais um no sistema.

- A Control Unit pode estar conectado fisicamente aos sensores e pumps.... ou só aos pumps !!!! Mas tem de haver redundancia ou no minimo lancar alarm se nao funcionar



### Respostas-2

- Os Hazards no documento sao um Stakeholder need. Por isso a lista da analise de safety pode ser entendido como requesitos.
- Ter duas bombas e um stakeholder need
- mapear o MoE a um requisito que diz para usar as duas bombas.

- Use case: o block com use case devia ligar a actores e nao a blocks que pertencem ao mesmo sistema.

- No final, o modelo que foi 'modelado' tem de bater certo com o prototipo. se nao for o caso, deve ser justificado

- No final, requisitos e diagrama e blocos com os elementos!!!!! No state machine

- Deployment diagram with own MQTT broker cluster or other protocol

- Cluster strategy.... load balancer to all nodes in cluster !!!!

### Why Critical

- Byzantine fault
- https://www.researchgate.net/publication/351550256_Smart_Scheduling_of_Pump_Control_in_Wastewater_Networks_Based_on_Electricity_Spot_Market_Prices/figures?lo=1
- Pull vs Push

- Se nao receber mensagem mqtt, questionar servidor quanto ao estado

- MQTT recupera ligaçoes ao contrario de tcp


-------------------------------------------

Implementation of a prototype:

Concorrency and Scheduling RT
  - Prof Claudio: descriç~ao das limitçoes e os cuidados
    - desenhar um diagrama
    - atribuir prioridades
    - configuraçao dos periodos
    - estimar o tempo de processaçao atraves do cpu que se usa e o numero de instruçoes
    - ler sensor, processar dados, atualizar estado, interacao com comunicacao....

  - mutex stuff in control unit
  - Control Unit, give one core for comunication and one core for processing
  - Mutex whe processing is writing the new state to be send
  - This is only descriptive 
  - https://www.esp32.com/viewtopic.php?t=2118

Baseline communication
  - network diagram

Assembly funcionality
  - bit manipulation: 00001 > 00010
  - 0000 : 0
  - 1000 : 8
  - 0100 : 4
  - 0010 : 2
  - 0001 : 1


Demostration
  Sensor sim + Control Unit MVP + Pump Led


  
