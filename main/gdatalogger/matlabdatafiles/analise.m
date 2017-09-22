clear
load('data.mat');
size = num2str(pitch_angle_size-1);
tsim = 5;
eval(['pitch_angle' '=' 'pitch_angle_0_' num2str(size)]);
eval(['roll_angle' '=' 'roll_angle_0_' num2str(size)]);
eval(['pitch_speed' '=' 'pitch_speed_0_' num2str(size)]);
eval(['roll_speed' '=' 'roll_speed_0_' num2str(size)]);
eval(['pitch_speed_sf' '=' 'pitch_speed_sf_0_' num2str(size)]);
eval(['roll_speed_sf' '=' 'roll_speed_sf_0_' num2str(size)]);

t = (0:length(pitch_angle)-1)*tsim/(length(pitch_angle)-1);

plot(t,pitch_angle);
hold on
plot(t,roll_angle);
legend('Arfagem','Rolagem');
title('Angulos x Tempo');
xlabel('Tempo (s)');
ylabel('Angulo (^o)');

figure

plot(t,pitch_speed);
hold on
plot(t,roll_speed);
legend('Arfagem','Rolagem');
title('Velocidade x Tempo');
xlabel('Tempo (s)');
ylabel('Velocidade (rad/s)');

figure

plot(t,pitch_speed_sf);
hold on
plot(t,roll_speed_sf);
legend('Arfagem','Rolagem');
title('Velocidade sem Filtro x Tempo');
xlabel('Tempo (s)');
ylabel('Velocidade (rad/s)');
