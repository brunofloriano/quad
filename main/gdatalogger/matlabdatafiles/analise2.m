clear all
close all
load('data.mat');
size = (pitch_angle_size-1);
for i=0:size
eval(['pitch_angle(i+1)' '=' 'pitch_angle_' num2str(i) '_' num2str(i) ';']);
eval(['roll_angle(i+1)' '=' 'roll_angle_' num2str(i) '_' num2str(i) ';']);
eval(['pitch_speed(i+1)' '=' 'pitch_speed_' num2str(i) '_' num2str(i) ';']);
eval(['roll_speed(i+1)' '=' 'roll_speed_' num2str(i) '_' num2str(i) ';']);
eval(['pitch_speed_sf(i+1)' '=' 'pitch_speed_sf_' num2str(i) '_' num2str(i) ';']);
eval(['roll_speed_sf(i+1)' '=' 'roll_speed_sf_' num2str(i) '_' num2str(i) ';']);
end

tsim = length(pitch_angle)*0.1;

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