clear all
close all
load('data.mat');
size = num2str(pitch_angle_size-1);
tsim = 10;
eval(['pitch_angle' '=' 'pitch_angle_0_' num2str(size) ';']);
eval(['roll_angle' '=' 'roll_angle_0_' num2str(size) ';']);
eval(['pitch_speed' '=' 'pitch_speed_0_' num2str(size) ';']);
eval(['roll_speed' '=' 'roll_speed_0_' num2str(size) ';']);

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


for i=1:12
size = num2str(v_motor1_size-1);
eval(['size' '=' 'num2str(v_motor' num2str(i) '_size -1);']);
eval(['v_motor' '=' 'v_motor' num2str(i) '_0_' num2str(size) ';']);
t = (0:length(v_motor)-1)*tsim/(length(v_motor)-1);
plot(t,v_motor);
hold on
end
legend('Motor1','Motor2','Motor3','Motor4','Motor5','Motor6','Motor7','Motor8','Motor9','Motor10','Motor11','Motor12');
title('Velocidade dos Motores x Tempo');
xlabel('Tempo (s)');
ylabel('Velocidade (rad/s)');