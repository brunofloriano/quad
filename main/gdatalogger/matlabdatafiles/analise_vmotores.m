clear all
close all
load('data.mat');
size = num2str(pitch_angle_size-1);
eval(['pitch_angle' '=' 'pitch_angle_0_' num2str(size) ';']);
eval(['roll_angle' '=' 'roll_angle_0_' num2str(size) ';']);
eval(['pitch_speed' '=' 'pitch_speed_0_' num2str(size) ';']);
eval(['roll_speed' '=' 'roll_speed_0_' num2str(size) ';']);

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

for i=1:12
if i == 1 || i == 4 || i == 7 || i == 10
size = num2str(v_motor1_size-1);
eval(['size' '=' 'num2str(v_motor' num2str(i) '_size -1);']);
eval(['v_motor' '=' 'v_motor' num2str(i) '_0_' num2str(size) ';']);
t = (0:length(v_motor)-1)*tsim/(length(v_motor)-1);
a=plot(t,v_motor);
set(a,'Color','yellow');
hold on
end
end
t = (0:length(pitch_angle)-1)*tsim/(length(pitch_angle)-1);
b=plot(t,roll_speed);
set(b,'Color','red');
title('Velocidade dos Motores Roll x Tempo');
xlabel('Tempo (s)');
ylabel('Velocidade (rad/s)');

figure

for i=1:12
if i == 2 || i == 3 || i == 5 || i == 6 || i == 8 || i == 9 || i == 11 || i == 12
size = num2str(v_motor1_size-1);
eval(['size' '=' 'num2str(v_motor' num2str(i) '_size -1);']);
eval(['v_motor' '=' 'v_motor' num2str(i) '_0_' num2str(size) ';']);
t = (0:length(v_motor)-1)*tsim/(length(v_motor)-1);
a=plot(t,v_motor);
set(a,'Color','yellow');
hold on
end
end
t = (0:length(pitch_angle)-1)*tsim/(length(pitch_angle)-1);
b=plot(t,pitch_speed);
set(b,'Color','red');
title('Velocidade dos Motores Pitch x Tempo');
xlabel('Tempo (s)');
ylabel('Velocidade (rad/s)');