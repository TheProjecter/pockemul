#ifndef ANALOG_H
#define ANALOG_H


typedef	struct 
{
	qint64	values;
	qint64	state;
	qint8	marker;
} TAnalog_Data;


class CData:public QObject
{
    public:
		void			Clear(void);
		void			Write(TAnalog_Data);
		void			Write(qint64,qint64);

		TAnalog_Data	Read(int);
		qint64			Read_values(int);
		qint64			Read_state(int);
		qint8			Read_marker(int);
		qint64			nb_data;
		int				size(void){ return values.size(); }

		int				Save(void);
		int				Load(void);
		
		CData()
		{
			maxSize = 1000000;
			Marker = 0;
		}
        virtual ~CData() {}
        
//    private:
    	int maxSize;
    	float				timeUnit;
    	qint8				Marker;
		QList<qint64>		values;
		QList<qint64>		state;
		QList<qint8>		marker;
		
	friend class dialogAnalog;
};

QDataStream &operator<<(QDataStream &out, const CData &dataplot);
QDataStream &operator>>(QDataStream &in, CData &dataplot);





#endif /* ANALOG_H */
