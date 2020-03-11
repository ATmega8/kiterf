//
// Created by life on 20-1-14.
//

#ifndef MSI001_CS5361_H
#define MSI001_CS5361_H
typedef enum {
   CS5361_MASTER_MODE,
   CS5361_SLAVE_MODE
} cs5361_mode_t;

typedef enum {
   CS5361_SINGLE_SPEED,
   CS5361_DOUBLE_SPEED,
   CS5361_QUAD_SPEED
} cs5361_speed_mode_t;

typedef enum {
   CS5361_MDIV_1,
   CS5361_MDIV_2
} cs5361_mdiv_mode_t;

typedef enum {
   CS5361_HPF_DISABLE,
   CS5361_HPF_ENABLE
} cs5361_hpf_mode_t;

typedef enum {
   CS5361_I2S_MODE,
   CS5361_LEFT_JUSTIFIED_MODE
} cs5361_sa_mode_t;

void cs5361_set_mode(cs5361_mode_t mode);
void cs5361_set_speed_mode(cs5361_speed_mode_t speed);
void cs5361_set_mdiv(cs5361_mdiv_mode_t mdiv);
void cs5361_set_hpf(cs5361_hpf_mode_t hpf);
void cs5361_set_sa(cs5361_sa_mode_t sa);
void cs5361_powerdown(void);
void cs5361_powerup(void);
void cs5361_init(void);
#endif //MSI001_CS5361_H
