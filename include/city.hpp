#ifndef CITY_H
#define CITY_H

class City {
public:
  City();
  City(int id, double coord_x, double coord_y);

  int getId() const;
  double getCoordX() const;
  double getCoordY() const;

  void display() const;

  // Calcule la distance euclidienne vers une autre ville
  double distanceTo(const City &other) const;

private:
  int id;
  double coord_x;
  double coord_y;
};

#endif
