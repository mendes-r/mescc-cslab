# Notes

## (RAMDE)

- projecto em papyrus
- mais um pdf com screenshots dos principais diagramas e justificações
    - diagrama de caso de uso

- Nao te esqueças do SERVIDOR durante a modelação

### QA

- porquê a existência de dois poços? existe uma relação entre eles?
    - se não é o caso, podemos apenas simular um dos poços? E no WWW simularuma fonte de informação falsa do segundo poço?

- O que acontece se o nível máximo de água é alcançada? a bomba para? Ou fechamos o input? Ou aumentamos a potência das bombas? Tendo em conta o caudal maximo do cano de input podemos ter bombas prontas para o máximo. 
ou apenas um wet weather overflow?

- Quando soa um alarme e depois manualmente clicamos no botão de disable, o que acontece?

- As remote status stations são por poço ou ambas mostram o estado de ambos os poços?

### Respostas

24-01-04

- Apenas uma bomba por WPS, contudo podemos tornar o systema mais complexo... se chegar a um nível médio trigger second pump... se uma bomba falhar, o nivel médio passa a ser o nível máximo.

- Quando o nível máximo é alcançado um alarme soa nos RSS. O butão apenas serve para calar o alarme. Nenhuma ação é necessária quando o alarme é acionado (nada de fechar inputs ou coisas semelhantes)

- Um RSS mostra os estado de todos os WSP

- No final de CSLAB podemos simular apenas um dos WSP, contudo temos de mostrar que é possível adicionar mais um no sistema.

- A Control Unit pode estar conectado fisicamente aos sensores e pumps.... ou só aos pumps !!!! Mas tem de haver redundancia ou no minimo lancar alarm se nao funcionar

### Why Critical

- Byzantine fault
- https://www.researchgate.net/publication/351550256_Smart_Scheduling_of_Pump_Control_in_Wastewater_Networks_Based_on_Electricity_Spot_Market_Prices/figures?lo=1
- Pull vs Push

- Se nao receber mensagem mqtt, questionar servidor quanto ao estado
- Talvez usar mqtt tb quando comunicar com os sensores

- MQTT recupera ligaçoes ao contrario de tcp
