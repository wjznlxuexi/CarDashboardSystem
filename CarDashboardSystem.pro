TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += final_simulator \
           dashboard \

final_simulator.file = final_simulator/final_simulator.pro
dashboard.file = dashboard/dashboard.pro

dashboard.depends = final_simulator
