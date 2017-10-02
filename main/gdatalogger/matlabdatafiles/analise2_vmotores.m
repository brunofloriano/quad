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

for i=2:12
if i == 1 || i == 4 || i == 7 || i == 10
eval(['size' '=' 'v_motor' num2str(i) '_size -1;']);
for j=0:size
eval(['v_motor(j+1)' '=' 'v_motor' num2str(i) '_' num2str(j) '_' num2str(j) ';']);
t = (0:length(v_motor)-1)*tsim/(length(v_motor)-1);
a=plot(t,v_motor);
set(a,'Color','yellow');
hold on
end
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
eval(['size' '=' 'v_motor' num2str(i) '_size -1;']);
for j=0:size
eval(['v_motor(j+1)' '=' 'v_motor' num2str(i) '_' num2str(j) '_' num2str(j) ';']);
t = (0:length(v_motor)-1)*tsim/(length(v_motor)-1);
a=plot(t,v_motor);
set(a,'Color','yellow');
hold on
end
end
end
t = (0:length(pitch_angle)-1)*tsim/(length(pitch_angle)-1);
b=plot(t,pitch_speed);
set(b,'Color','red');
title('Velocidade dos Motores Pitch x Tempo');
xlabel('Tempo (s)');
ylabel('Velocidade (rad/s)');