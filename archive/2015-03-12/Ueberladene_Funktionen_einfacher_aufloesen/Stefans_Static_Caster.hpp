
// "Static Caster"
template <typename... ArgsFkt>
class SC{
public:
	// free function
	template<typename Returntype>
	using Fkt = Returntype (*) (ArgsFkt...);

	template<typename Returntype, typename Classtype>
	Fkt<Returntype> operator()(Fkt<Returntype> fkt){
		return fkt;
	}

	// (non-constant, non-volatile) member function
	template<typename Returntype, typename Classtype>
	using Fkt_m = Returntype (Classtype::*) (ArgsFkt...);

	template<typename Returntype, typename Classtype>
	Fkt_m<Returntype, Classtype> operator()(Fkt_m<Returntype, Classtype> fkt){
		return fkt;
	}

	// constant member function
	template<typename Returntype, typename Classtype>
	using Fkt_m_c = Returntype (Classtype::*) (ArgsFkt...) const;

	template<typename Returntype, typename Classtype>
	Fkt_m_c<Returntype, Classtype> operator()(Fkt_m_c<Returntype, Classtype> fkt){
		return fkt;
	}

	// volatile member function
	template<typename Returntype, typename Classtype>
	using Fkt_m_v = Returntype (Classtype::*) (ArgsFkt...) volatile;

	template<typename Returntype, typename Classtype>
	Fkt_m_v<Returntype, Classtype> operator()(Fkt_m_v<Returntype, Classtype> fkt){
		return fkt;
	}

	// constant, volatile member function
	template<typename Returntype, typename Classtype>
	using Fkt_m_c_v = Returntype (Classtype::*) (ArgsFkt...) const volatile;

	template<typename Returntype, typename Classtype>
	Fkt_m_c_v<Returntype, Classtype> operator()(Fkt_m_c_v<Returntype, Classtype> fkt){
		return fkt;
	}
};

/* 
void test(){
	// Qt5-Aufruf normal
	connect(
		spin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		label, static_cast<void(QLabel::*)(int)>(&QLabel::setNum));

	// Qt5-Aufruf mit Klasse SC
	connect(
		spin, SC<int>()(&QSpinBox::valueChanged),
		label, SC<int>()(&QLabel::setNum));
}
*/
