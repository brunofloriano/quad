load('data.mat');
c = '44';
tsim = 5;
t = (0:length(pitch_angle_0_42)-1)*tsim/(length(pitch_angle_0_42)-1);
plot(t,pitch_angle_0_42);
hold on
plot(t,roll_angle_0_42);
legend('Arfagem','Rolagem');
title('Angulos x Tempo');
xlabel('Tempo (s)');
ylabel('Angulo (^o)');

figure

plot(t,pitch_speed_0_42);
hold on
plot(t,roll_speed_0_42);
legend('Arfagem','Rolagem');
title('Velocidade x Tempo');
xlabel('Tempo (s)');
ylabel('Velocidade (rad/s)');
